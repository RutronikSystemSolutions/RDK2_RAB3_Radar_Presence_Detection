using OxyPlot.Axes;
using OxyPlot;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using OxyPlot.Series;

namespace RDK2_Visualisation_GUI
{
    public partial class RadarDetectionView : UserControl
    {
        private class DataWithTimeStamp
        {
            public double x;
            public double y;
            public double magnitude;
            public double timestamp;

            public DataWithTimeStamp(double x, double y, double magnitude)
            {
                this.x = x;
                this.y = y;
                this.magnitude = magnitude;
                timestamp = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            }
        }

        private List<DataWithTimeStamp> data = new List<DataWithTimeStamp>();
        private System.Timers.Timer timer = new System.Timers.Timer();

        /// <summary>
        /// X Axis
        /// </summary>
        LinearAxis xAxis = new LinearAxis
        {
            MajorGridlineStyle = LineStyle.Dot,
            Position = AxisPosition.Bottom,
            AxislineStyle = LineStyle.Solid,
            AxislineColor = OxyColors.Gray,
            FontSize = 10,
            Minimum = -10,
            Maximum = 10,
            PositionAtZeroCrossing = true,
            IsPanEnabled = false,
            IsZoomEnabled = true,
            Unit = "X"
        };

        /// <summary>
        /// Y Axis
        /// </summary>
        private LinearAxis yAxis = new LinearAxis
        {
            MajorGridlineStyle = LineStyle.Dot,
            AxislineStyle = LineStyle.Solid,
            AxislineColor = OxyColors.Gray,
            FontSize = 10,
            TextColor = OxyColors.Gray,
            Position = AxisPosition.Left,
            IsPanEnabled = false,
            IsZoomEnabled = true,
            Minimum = 0,
            Maximum = 10,
            Unit = "Y"
        };

        private ScatterSeries detectedPresenceSeries = new ScatterSeries();
        private ScatterSeries lastValueSeries = new ScatterSeries();

        public RadarDetectionView()
        {
            InitializeComponent();
            InitPlot();

            timer.Interval = 100;
            timer.Elapsed += Timer_Elapsed;
            timer.Start();
        }

        private void Timer_Elapsed(object? sender, System.Timers.ElapsedEventArgs e)
        {
            // Only show the last x seconds
            double timeMs = DateTimeOffset.Now.ToUnixTimeMilliseconds();

            double deadline = timeMs - 10000;

            // First delele (points are sorted)
            int deleteCount = data.Count;
            for (int i = 0; i < data.Count; ++i)
            {
                if (data[i].timestamp > deadline)
                {
                    deleteCount = i;
                    break;
                }
            }
            data.RemoveRange(0, deleteCount);


            detectedPresenceSeries.Points.Clear();
            for(int i = 0; i < data.Count; ++i)
            {
                detectedPresenceSeries.Points.Add(new ScatterPoint(x: data[i].x, y: data[i].y, value: data[i].magnitude));
            }

            lastValueSeries.Points.Clear();
            if (data.Count > 0)
            {
                lastValueSeries.Points.Add(new ScatterPoint(x: data[data.Count - 1].x, y: data[data.Count - 1].y, value: data[data.Count - 1].magnitude));
            }

            plotView.InvalidatePlot(true);
        }

        private void InitPlot()
        {
            // Raw signals plot
            var timeModel = new PlotModel
            {
                PlotType = PlotType.XY,
                PlotAreaBorderThickness = new OxyThickness(0),
            };

            // Set the axes
            timeModel.Axes.Add(xAxis);
            timeModel.Axes.Add(yAxis);

            var axis1 = new LinearColorAxis();
            axis1.Key = "ColorAxis";
            axis1.Maximum = 32;
            axis1.Minimum = 0;
            axis1.Position = AxisPosition.Top;
            timeModel.Axes.Add(axis1);

            // Add series
            detectedPresenceSeries.Title = "Presence";
            detectedPresenceSeries.ColorAxisKey = "ColorAxis";

            lastValueSeries.MarkerSize = 10;
            lastValueSeries.MarkerFill = OxyColors.Red;
            lastValueSeries.MarkerStroke = OxyColors.Black;

            timeModel.Series.Add(detectedPresenceSeries);
            timeModel.Series.Add(lastValueSeries);

            plotView.Model = timeModel;
            plotView.InvalidatePlot(true);
        }

        public void SignalPresenceDetected(double magnitude, double range, double angle)
        {
            // Apply +90° shift on the angle (Pi / 2)
            angle += Math.PI / 2;

            // Compute X and Y
            double x = range * Math.Cos(angle);
            double y = range * Math.Sin(angle);

            data.Add(new DataWithTimeStamp(x, y, magnitude));
        }
    }
}
