namespace ConvertWavToByteArray
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.selectButton = new System.Windows.Forms.Button();
            this.openLocation = new System.Windows.Forms.TextBox();
            this.convertButton = new System.Windows.Forms.Button();
            this.bitPerSampleLabel = new System.Windows.Forms.Label();
            this.samplingLabel = new System.Windows.Forms.Label();
            this.bitRateButton = new System.Windows.Forms.Label();
            this.sampleRateBox = new System.Windows.Forms.TextBox();
            this.byteRateBox = new System.Windows.Forms.TextBox();
            this.bitsPerSampleBox = new System.Windows.Forms.TextBox();
            this.saveButton = new System.Windows.Forms.Button();
            this.displayBox = new System.Windows.Forms.TabControl();
            this.hexPage = new System.Windows.Forms.TabPage();
            this.hexTextBox = new System.Windows.Forms.RichTextBox();
            this.decimalPage = new System.Windows.Forms.TabPage();
            this.decTextBox = new System.Windows.Forms.RichTextBox();
            this.playButton = new System.Windows.Forms.Button();
            this.displayBox.SuspendLayout();
            this.hexPage.SuspendLayout();
            this.decimalPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // selectButton
            // 
            this.selectButton.Location = new System.Drawing.Point(307, 12);
            this.selectButton.Name = "selectButton";
            this.selectButton.Size = new System.Drawing.Size(75, 23);
            this.selectButton.TabIndex = 0;
            this.selectButton.Text = "Select File";
            this.selectButton.UseVisualStyleBackColor = true;
            this.selectButton.Click += new System.EventHandler(this.selectButton_Click);
            // 
            // openLocation
            // 
            this.openLocation.Enabled = false;
            this.openLocation.Location = new System.Drawing.Point(13, 14);
            this.openLocation.Name = "openLocation";
            this.openLocation.ReadOnly = true;
            this.openLocation.Size = new System.Drawing.Size(288, 20);
            this.openLocation.TabIndex = 2;
            // 
            // convertButton
            // 
            this.convertButton.Location = new System.Drawing.Point(307, 41);
            this.convertButton.Name = "convertButton";
            this.convertButton.Size = new System.Drawing.Size(75, 23);
            this.convertButton.TabIndex = 4;
            this.convertButton.Text = "Convert";
            this.convertButton.UseVisualStyleBackColor = true;
            this.convertButton.Click += new System.EventHandler(this.convertButton_Click);
            // 
            // bitPerSampleLabel
            // 
            this.bitPerSampleLabel.AutoSize = true;
            this.bitPerSampleLabel.Location = new System.Drawing.Point(12, 83);
            this.bitPerSampleLabel.Name = "bitPerSampleLabel";
            this.bitPerSampleLabel.Size = new System.Drawing.Size(81, 13);
            this.bitPerSampleLabel.TabIndex = 8;
            this.bitPerSampleLabel.Text = "Bits Per Sample";
            // 
            // samplingLabel
            // 
            this.samplingLabel.AutoSize = true;
            this.samplingLabel.Location = new System.Drawing.Point(12, 41);
            this.samplingLabel.Name = "samplingLabel";
            this.samplingLabel.Size = new System.Drawing.Size(76, 13);
            this.samplingLabel.TabIndex = 9;
            this.samplingLabel.Text = "Sampling Rate";
            // 
            // bitRateButton
            // 
            this.bitRateButton.AutoSize = true;
            this.bitRateButton.Location = new System.Drawing.Point(12, 62);
            this.bitRateButton.Name = "bitRateButton";
            this.bitRateButton.Size = new System.Drawing.Size(54, 13);
            this.bitRateButton.TabIndex = 10;
            this.bitRateButton.Text = "Byte Rate";
            // 
            // sampleRateBox
            // 
            this.sampleRateBox.Enabled = false;
            this.sampleRateBox.Location = new System.Drawing.Point(94, 38);
            this.sampleRateBox.Name = "sampleRateBox";
            this.sampleRateBox.ReadOnly = true;
            this.sampleRateBox.Size = new System.Drawing.Size(100, 20);
            this.sampleRateBox.TabIndex = 11;
            // 
            // byteRateBox
            // 
            this.byteRateBox.Enabled = false;
            this.byteRateBox.Location = new System.Drawing.Point(94, 59);
            this.byteRateBox.Name = "byteRateBox";
            this.byteRateBox.ReadOnly = true;
            this.byteRateBox.Size = new System.Drawing.Size(100, 20);
            this.byteRateBox.TabIndex = 12;
            // 
            // bitsPerSampleBox
            // 
            this.bitsPerSampleBox.Enabled = false;
            this.bitsPerSampleBox.Location = new System.Drawing.Point(94, 80);
            this.bitsPerSampleBox.Name = "bitsPerSampleBox";
            this.bitsPerSampleBox.ReadOnly = true;
            this.bitsPerSampleBox.Size = new System.Drawing.Size(100, 20);
            this.bitsPerSampleBox.TabIndex = 13;
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(306, 70);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(75, 23);
            this.saveButton.TabIndex = 1;
            this.saveButton.Text = "Save File";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // displayBox
            // 
            this.displayBox.Controls.Add(this.hexPage);
            this.displayBox.Controls.Add(this.decimalPage);
            this.displayBox.Location = new System.Drawing.Point(15, 106);
            this.displayBox.Name = "displayBox";
            this.displayBox.SelectedIndex = 0;
            this.displayBox.Size = new System.Drawing.Size(366, 152);
            this.displayBox.TabIndex = 14;
            // 
            // hexPage
            // 
            this.hexPage.Controls.Add(this.hexTextBox);
            this.hexPage.Location = new System.Drawing.Point(4, 22);
            this.hexPage.Name = "hexPage";
            this.hexPage.Padding = new System.Windows.Forms.Padding(3);
            this.hexPage.Size = new System.Drawing.Size(358, 126);
            this.hexPage.TabIndex = 0;
            this.hexPage.Text = "Hex Format";
            this.hexPage.UseVisualStyleBackColor = true;
            // 
            // hexTextBox
            // 
            this.hexTextBox.Location = new System.Drawing.Point(0, 0);
            this.hexTextBox.Name = "hexTextBox";
            this.hexTextBox.Size = new System.Drawing.Size(358, 126);
            this.hexTextBox.TabIndex = 0;
            this.hexTextBox.Text = "";
            // 
            // decimalPage
            // 
            this.decimalPage.Controls.Add(this.decTextBox);
            this.decimalPage.Location = new System.Drawing.Point(4, 22);
            this.decimalPage.Name = "decimalPage";
            this.decimalPage.Padding = new System.Windows.Forms.Padding(3);
            this.decimalPage.Size = new System.Drawing.Size(358, 126);
            this.decimalPage.TabIndex = 1;
            this.decimalPage.Text = "Decimal Format";
            this.decimalPage.UseVisualStyleBackColor = true;
            // 
            // decTextBox
            // 
            this.decTextBox.Location = new System.Drawing.Point(0, 0);
            this.decTextBox.Name = "decTextBox";
            this.decTextBox.Size = new System.Drawing.Size(358, 126);
            this.decTextBox.TabIndex = 0;
            this.decTextBox.Text = "";
            // 
            // playButton
            // 
            this.playButton.Location = new System.Drawing.Point(306, 99);
            this.playButton.Name = "playButton";
            this.playButton.Size = new System.Drawing.Size(75, 23);
            this.playButton.TabIndex = 15;
            this.playButton.Text = "Play Audio";
            this.playButton.UseVisualStyleBackColor = true;
            this.playButton.Click += new System.EventHandler(this.playButton_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(393, 270);
            this.Controls.Add(this.playButton);
            this.Controls.Add(this.displayBox);
            this.Controls.Add(this.bitsPerSampleBox);
            this.Controls.Add(this.byteRateBox);
            this.Controls.Add(this.sampleRateBox);
            this.Controls.Add(this.bitRateButton);
            this.Controls.Add(this.samplingLabel);
            this.Controls.Add(this.bitPerSampleLabel);
            this.Controls.Add(this.convertButton);
            this.Controls.Add(this.openLocation);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.selectButton);
            this.Name = "MainForm";
            this.Text = "WavToByteConverter";
            this.displayBox.ResumeLayout(false);
            this.hexPage.ResumeLayout(false);
            this.decimalPage.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button selectButton;
        private System.Windows.Forms.TextBox openLocation;
        private System.Windows.Forms.Button convertButton;
        private System.Windows.Forms.Label bitPerSampleLabel;
        private System.Windows.Forms.Label samplingLabel;
        private System.Windows.Forms.Label bitRateButton;
        private System.Windows.Forms.TextBox sampleRateBox;
        private System.Windows.Forms.TextBox byteRateBox;
        private System.Windows.Forms.TextBox bitsPerSampleBox;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.TabControl displayBox;
        private System.Windows.Forms.TabPage hexPage;
        private System.Windows.Forms.TabPage decimalPage;
        private System.Windows.Forms.RichTextBox hexTextBox;
        private System.Windows.Forms.RichTextBox decTextBox;
        private System.Windows.Forms.Button playButton;
    }
}

