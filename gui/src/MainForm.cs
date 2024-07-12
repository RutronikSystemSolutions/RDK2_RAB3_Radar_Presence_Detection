using System.IO.Ports;

namespace RDK2_Visualisation_GUI
{
    public partial class MainForm : Form
    {
        private RDK2Listener rdk2 = new RDK2Listener();

        public MainForm()
        {
            InitializeComponent();
            rdk2.OnNewConnectionState += Rdk2_OnNewConnectionState;
            rdk2.OnNewPacket += Rdk2_OnNewPacket;
        }


        private void Rdk2_OnNewPacket(object sender, RDK2Event rdk2Event)
        {
            System.Diagnostics.Debug.WriteLine(string.Format("{0} {1}m {2}°", rdk2Event.magnitude, rdk2Event.range, rdk2Event.angle));
            radarDetectionView.SignalPresenceDetected(rdk2Event.magnitude, rdk2Event.range, rdk2Event.angle);
        }

        private void Rdk2_OnNewConnectionState(object sender, RDK2Listener.ConnectionState state)
        {
            switch (state)
            {
                case RDK2Listener.ConnectionState.Connected:
                    {
                        connectButton.Enabled = false;
                        disconnectButton.Enabled = true;
                        break;
                    }
                case RDK2Listener.ConnectionState.Iddle:
                    {
                        connectButton.Enabled = true;
                        disconnectButton.Enabled = false;
                        break;
                    }
                case RDK2Listener.ConnectionState.Error:
                    {
                        connectButton.Enabled = true;
                        disconnectButton.Enabled = false;
                        break;
                    }
            }
        }

        /// <summary>
        /// Form loaded for the first time
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainForm_Load(object sender, EventArgs e)
        {
            // Load the possible com ports
            string[] serialPorts = SerialPort.GetPortNames();
            portComboBox.DataSource = serialPorts;
        }

        /// <summary>
        /// Event handler: click on the "Connect" button
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void connectButton_Click(object sender, EventArgs e)
        {
            if ((portComboBox.SelectedIndex < 0) || (portComboBox.SelectedIndex >= portComboBox.Items.Count)) return;

            var selectedItem = portComboBox.Items[portComboBox.SelectedIndex];
            if (selectedItem != null)
            {
                string? portName = selectedItem.ToString();
                if (portName != null) rdk2.SetPortName(portName);
            }
        }
    }
}
