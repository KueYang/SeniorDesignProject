using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace ConvertWavToByteArray
{
    public partial class MainForm : Form
    {
        string file, wavDataHex, wavDataDec;
        byte[] wav;
        List<byte> wavHeader, wavRawData;

        public MainForm()
        {
            InitializeComponent();
            wavHeader = new List<byte>();
            wavRawData = new List<byte>();
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = ".csv|*.csv";
            saveFileDialog.Title = "Save CSV file";
            saveFileDialog.ShowDialog();

            if(saveFileDialog.FileName != "")
            {
                string filePath = saveFileDialog.FileName;
                try
                {
                    if (this.displayBox.SelectedIndex == 0)
                    {
                        wavDataHex = wavDataHex.Replace('-', ',');
                        File.WriteAllText(filePath, wavDataHex);
                    }
                    else
                    {
                        File.WriteAllText(filePath, wavDataDec);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error: Could not write to file. Original error: " + ex.Message);
                }
            }
            else
            {
                MessageBox.Show("Error: File path cannot be empty.");
            }
        }

        private void selectButton_Click(object sender, EventArgs e)
        {
            // Create an instance of the open file dialog box.
            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            // Set filter options and filter index.
            openFileDialog1.Filter = "WAV files (.wav)|*.wav";
            openFileDialog1.FilterIndex = 1;
            openFileDialog1.Multiselect = false;

            // Process input if the user clicked OK.
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    if(openFileDialog1.CheckFileExists && openFileDialog1.CheckPathExists)
                    {
                        file = openFileDialog1.FileName;
                        this.openLocation.Text = file;
                    }
                    else
                    {
                        MessageBox.Show("Error: File doesn't exist and/or path could not be found.");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
                }
            }
        }

        private void convertButton_Click(object sender, EventArgs e)
        {
            if(File.Exists(file))
            {
                try
                {
                    wav = File.ReadAllBytes(file);

                    // Parses the first 44 bytes for the wav header.
                    for (int i = 0; i < 44; i++)
                    {
                        wavHeader.Add(wav[i]);
                    }

                    byte[] headerArray = wavHeader.ToArray();
                    Int32 bitPerSample = (headerArray[35] << 8 | headerArray[34]);
                    Int32 byteRate = (headerArray[31] << 24 | headerArray[30] << 16 | headerArray[29] << 8 | headerArray[28]);
                    Int32 sampleRate = (headerArray[27] << 24 | headerArray[26] << 16 | headerArray[25] << 8 | headerArray[24]);

                    this.sampleRateBox.Text = sampleRate.ToString();
                    this.byteRateBox.Text = byteRate.ToString();
                    this.bitsPerSampleBox.Text = bitPerSample.ToString();


                    // Parses the data from the wav file.
                    wavRawData = new List<byte>();
                    for (int i = 44; i < (wav.Length); i++)
                    {
                        wavRawData.Add(wav[i]);
                    }

                    byte[] wavData = wavRawData.ToArray();
                    wavDataHex = BitConverter.ToString(wavData);
                    wavDataDec = string.Join(",", Array.ConvertAll<byte, string>(wavData, x => x.ToString()));

                    this.hexTextBox.Text = wavDataHex;
                    this.decTextBox.Text = wavDataDec;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error: Could not parse wav data. Original error: " + ex.Message);
                }
            }
            else
            {
                MessageBox.Show("Error: File doesn't exist and/or path could not be found.");
            }
        }
    }
}
