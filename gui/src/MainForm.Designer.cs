namespace RDK2_Visualisation_GUI
{
    partial class MainForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            portComboBox = new ComboBox();
            label1 = new Label();
            connectButton = new Button();
            disconnectButton = new Button();
            radarDetectionView = new RadarDetectionView();
            SuspendLayout();
            // 
            // portComboBox
            // 
            portComboBox.FormattingEnabled = true;
            portComboBox.Location = new Point(96, 12);
            portComboBox.Name = "portComboBox";
            portComboBox.Size = new Size(151, 28);
            portComboBox.TabIndex = 1;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(13, 15);
            label1.Name = "label1";
            label1.Size = new Size(77, 20);
            label1.TabIndex = 2;
            label1.Text = "COM port:";
            // 
            // connectButton
            // 
            connectButton.Location = new Point(253, 11);
            connectButton.Name = "connectButton";
            connectButton.Size = new Size(94, 29);
            connectButton.TabIndex = 3;
            connectButton.Text = "Connect";
            connectButton.UseVisualStyleBackColor = true;
            connectButton.Click += connectButton_Click;
            // 
            // disconnectButton
            // 
            disconnectButton.Enabled = false;
            disconnectButton.Location = new Point(353, 11);
            disconnectButton.Name = "disconnectButton";
            disconnectButton.Size = new Size(94, 29);
            disconnectButton.TabIndex = 4;
            disconnectButton.Text = "Disconnect";
            disconnectButton.UseVisualStyleBackColor = true;
            // 
            // radarDetectionView
            // 
            radarDetectionView.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            radarDetectionView.BorderStyle = BorderStyle.FixedSingle;
            radarDetectionView.Location = new Point(13, 46);
            radarDetectionView.Name = "radarDetectionView";
            radarDetectionView.Size = new Size(1277, 679);
            radarDetectionView.TabIndex = 5;
            // 
            // MainForm
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(1302, 737);
            Controls.Add(radarDetectionView);
            Controls.Add(disconnectButton);
            Controls.Add(connectButton);
            Controls.Add(label1);
            Controls.Add(portComboBox);
            Name = "MainForm";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "RDK2 - RAB3 - Presence Detection Visualisation";
            Load += MainForm_Load;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ComboBox portComboBox;
        private Label label1;
        private Button connectButton;
        private Button disconnectButton;
        private RadarDetectionView radarDetectionView;
    }
}
