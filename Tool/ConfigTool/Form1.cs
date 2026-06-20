using System;
using System.Windows.Forms;
using System.Net;
using System.IO;
using System.Text;

namespace ConfigTool
{
    public partial class ConfigTool : Form
    {
        ConfigData loader;
        ConfigData.locationData selectedData;
        public ConfigTool()
        {
            InitializeComponent();
            loader = new ConfigData();
            if (loader.LoadConfigFile("config.json"))
            {
                ServiceTypeCombo.Items.Add("QA");
                ServiceTypeCombo.Items.Add("Live");
                LocationCombo.Items.Clear();
            }
            else
            {
                MessageBox.Show("Config.json 파일 로드 실패");
            }
        }

        // 서비스 타입 변경
        private void ServiceTypeCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            LocationCombo.Items.Clear();

            string val = ServiceTypeCombo.SelectedItem as string;
            if ("QA" == val)
            {
                foreach(var data in loader.QAData)
                    LocationCombo.Items.Add(data.name);
            }
            else if( "Live" == val)
            {
                foreach (var data in loader.LiveData)
                    LocationCombo.Items.Add(data.name);
            }
        }
        // 지역 변경
        private void LocationCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            string loc = LocationCombo.SelectedItem as string;
            string type = ServiceTypeCombo.SelectedItem as string;
            selectedData = loader.GetData(type, loc);

            ConfigViewer.Clear();
            if (selectedData.IsEnbale())
            {
                string ftpPath = "ftp://" + selectedData.ip + ":" + selectedData.port + "/" + selectedData.path + "/" + selectedData.filename;
                FtpWebRequest req = (FtpWebRequest)WebRequest.Create(new Uri(ftpPath));
                req.Method = WebRequestMethods.Ftp.DownloadFile;
                req.Credentials = new NetworkCredential(selectedData.id, selectedData.pw);
                using (FtpWebResponse resp = (FtpWebResponse)req.GetResponse())
                {
                    Stream stream = resp.GetResponseStream();

                    using (StreamReader reader = new StreamReader(stream))
                    {
                        ConfigViewer.Text = reader.ReadToEnd();
                    }
                }
            }
            else
            {
                MessageBox.Show("Config 정보가 없거나 잘못 됬습니다.");
            }
        }

        // Ftp Config file Downlaod
        private void DownloadBtn_Click(object sender, EventArgs e)
        {
            ConfigViewer.Clear();
            string ftpPath = "ftp://" + selectedData.ip + ":" + selectedData.port + "/" + selectedData.path + "/" + selectedData.filename;
            FtpWebRequest req = (FtpWebRequest)WebRequest.Create(new Uri(ftpPath));
            req.Method = WebRequestMethods.Ftp.DownloadFile;
            req.Credentials = new NetworkCredential(selectedData.id, selectedData.pw);
            using (FtpWebResponse resp = (FtpWebResponse)req.GetResponse())
            {
                Stream stream = resp.GetResponseStream();
       
                using (StreamReader reader = new StreamReader(stream))
                {
                    ConfigViewer.Text = reader.ReadToEnd();
                }
            }
        }        
    
        // Ftp Config File update
        private void UpdateBtn_Click(object sender, EventArgs e)
        {
            if (selectedData.IsEnbale())
            {
                string ftpPath = "ftp://" + selectedData.ip + ":" + selectedData.port + "/" + selectedData.path + "/" + selectedData.filename;
                FtpWebRequest req = (FtpWebRequest)WebRequest.Create(new Uri(ftpPath));
                req.Method = WebRequestMethods.Ftp.UploadFile;
                req.Credentials = new NetworkCredential(selectedData.id, selectedData.pw);

                byte[] data;
                data = Encoding.UTF8.GetBytes(ConfigViewer.Text);
                req.ContentLength = data.Length;
                using (Stream reqStream = req.GetRequestStream())
                {
                    reqStream.Write(data, 0, data.Length);
                }

                using (FtpWebResponse resp = (FtpWebResponse)req.GetResponse())
                {                    
                    MessageBox.Show("Upload Success");
                }
            }
            else
            {
                MessageBox.Show("Config 정보가 없거나 잘못 됬습니다.");
            }
        }
    }
}
