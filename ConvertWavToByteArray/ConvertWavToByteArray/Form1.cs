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
using System.Media;

namespace ConvertWavToByteArray
{
    public partial class MainForm : Form
    {
        string file, wavDataHex, wavDataDec;
        List<byte> wav;
        List<Int32> wavHeader, wavRawData;
        SoundPlayer audioPlayer;
        OpenFileDialog openFileDialog1;
        SaveFileDialog saveFileDialog;

        public MainForm()
        {
            InitializeComponent();
            
            openFileDialog1 = new OpenFileDialog();
            openFileDialog1.Filter = "WAV files (.wav)|*.wav";
            openFileDialog1.FilterIndex = 1;
            openFileDialog1.Multiselect = false;

            saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = ".csv|*.csv";
            saveFileDialog.Title = "Save CSV file";
        }

        private void playButton_Click(object sender, EventArgs e)
        {
            if(wav != null)
            {
                using (MemoryStream ms = new MemoryStream(wav.ToArray()))
                {
                    audioPlayer = new SoundPlayer(ms);
                    audioPlayer.Play();
                }
            }
            else
            {
                MessageBox.Show("Error: Need to select a file and convert prior to playing audio.");
            }
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != "")
            {
                string filePath = saveFileDialog.FileName;
                try
                {
                    if (this.displayBox.SelectedIndex == 0)
                    {
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
        }

        private void selectButton_Click(object sender, EventArgs e)
        {
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
                    wav = new List<byte>();
                    wavHeader = new List<Int32>();
                    wavRawData = new List<Int32>();

                    // Read bytes from .wav file
                    wav.AddRange(File.ReadAllBytes(file));
                    byte[] wavArray = wav.ToArray();

                    // Parses the first 44 bytes for the wav header.
                    for (int i = 0; i < 44; i++)
                    {
                        wavHeader.Add(wavArray[i]);
                    }

                    Int32[] headerArray = wavHeader.ToArray();
                    int bitPerSample = (headerArray[35] << 8 | headerArray[34]);
                    int byteRate = (headerArray[31] << 24 | headerArray[30] << 16 | headerArray[29] << 8 | headerArray[28]);
                    int sampleRate = (headerArray[27] << 24 | headerArray[26] << 16 | headerArray[25] << 8 | headerArray[24]);
                    int numOfChan = (headerArray[23] << 8 | headerArray[22]);

                    this.sampleRateBox.Text = sampleRate.ToString();
                    this.byteRateBox.Text = byteRate.ToString();
                    this.bitsPerSampleBox.Text = bitPerSample.ToString();
                    this.numOfChanBox.Text = numOfChan.ToString();

                    // Parses the data from the wav file.
                    for (int i = 44; i < (wavArray.Length); i++)
                    {
                        wavRawData.Add(wavArray[i]);
                    }

                    Int32[] wavData = wavRawData.ToArray();
                    wavDataHex = string.Join(",", Array.ConvertAll<Int32, string>(wavData, x => x.ToString("X")));
                    wavDataDec = string.Join(",", Array.ConvertAll<Int32, string>(wavData, x => x.ToString("D")));

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
