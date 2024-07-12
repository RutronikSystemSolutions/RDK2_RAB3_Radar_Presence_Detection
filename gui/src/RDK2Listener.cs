using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RDK2_Visualisation_GUI
{
    public class RDK2Listener
    {
        public enum ConnectionState
        {
            Iddle,
            Connected,
            Error
        }

        #region "Constants"

        private const int WorkerReportEvent = 1;

        #endregion

        #region "Events"

        public delegate void OnNewConnectionStateEventHandler(object sender, ConnectionState state);
        public event OnNewConnectionStateEventHandler? OnNewConnectionState;

        public delegate void OnNewPacketEventHandler(object sender, RDK2Event rdk2Event);
        public event OnNewPacketEventHandler? OnNewPacket;

        #endregion

        #region "Members"

        /// <summary>
        /// Serial port used for the communication
        /// </summary>
        private SerialPort? port = null;

        /// <summary>
        /// Background worker enabling background operations
        /// </summary>
        private BackgroundWorker? worker;

        /// <summary>
        /// Object used for synchronizsation purpose
        /// </summary>
        private object sync = new object();

        #endregion

        /// <summary>
        /// Set the serial port name
        /// </summary>
        /// <param name="portName"></param>
        public void SetPortName(string portName)
        {
            try
            {
                port = new SerialPort
                {
                    BaudRate = 115200,
                    DataBits = 8,
                    Handshake = Handshake.None,
                    Parity = Parity.None,
                    PortName = portName,
                    StopBits = StopBits.One,
                    ReadTimeout = 500,
                    WriteTimeout = 2000
                };
                port.Open();
            }
            catch (Exception)
            {
                OnNewConnectionState?.Invoke(this, ConnectionState.Error);
                return;
            }

            OnNewConnectionState?.Invoke(this, ConnectionState.Connected);

            CreateBackgroundWorkerAndStart();
        }

        private void CreateBackgroundWorkerAndStart()
        {
            if (worker != null)
            {
                worker.CancelAsync();
            }

            worker = new BackgroundWorker();
            worker.WorkerReportsProgress = true;
            worker.WorkerSupportsCancellation = true;
            worker.DoWork += Worker_DoWork;
            worker.ProgressChanged += Worker_ProgressChanged;
            worker.RunWorkerCompleted += Worker_RunWorkerCompleted;
            worker.RunWorkerAsync();
        }

        private void Worker_DoWork(object? sender, DoWorkEventArgs e)
        {
            if (sender == null) return;
            if (port == null) return;

            BackgroundWorker worker = (BackgroundWorker)sender;

            port.ReadExisting();

            const int bytesPerPacket = RDK2Event.PacketLength;
            byte[] readBuffer = new byte[bytesPerPacket];

            int offset = 0;
            int remaining = bytesPerPacket;

            for (; ; )
            {
                int available = port.BytesToRead;
                if (available > 0)
                {
                    int toRead = (available > remaining) ? remaining : available;
                    port.Read(readBuffer, offset, toRead);

                    offset += toRead;
                    remaining -= toRead;
                }

                if (remaining == 0)
                {
                    // Frame is available
                    System.Diagnostics.Debug.WriteLine("Available");
                    RDK2Event? rdk2Event = RDK2Event.Create(readBuffer);
                    if (rdk2Event != null)
                    {
                        worker.ReportProgress(WorkerReportEvent, rdk2Event);
                    }
                    else
                    {
                        System.Diagnostics.Debug.WriteLine("Packet was wrong...");
                        port.ReadExisting();
                    }
                    

                    offset = 0;
                    remaining = bytesPerPacket;
                }
            }
        }

        private void Worker_ProgressChanged(object? sender, ProgressChangedEventArgs e)
        {
            switch (e.ProgressPercentage)
            {
                case WorkerReportEvent:
                    {
                        RDK2Event? rdk2Event = e.UserState as RDK2Event;
                        if (rdk2Event != null)
                        {
                            OnNewPacket?.Invoke(this, rdk2Event);
                        }
                        break;
                    }
            }
        }

        private void Worker_RunWorkerCompleted(object? sender, RunWorkerCompletedEventArgs e)
        {
            OnNewConnectionState?.Invoke(this, ConnectionState.Iddle);
        }
    }
}
