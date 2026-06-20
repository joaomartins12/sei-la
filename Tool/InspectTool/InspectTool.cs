
using System;
using System.Text;
using System.Windows.Forms;
using Newtonsoft.Json.Linq;
using System.Net;
using System.IO;


namespace InspectTool
{
    public partial class InspectTool : Form
    {
        ConfigData loader;
        ConfigData.locationData selectedData;
        JObject jobj;

        public InspectTool()
        {
            InitializeComponent();
            this.Text += " Ver0.1";

            UpdateBtn.Enabled = false;
            loader = new ConfigData();
            if (loader.LoadConfigFile("config.json"))
            {
                ServiceType.Items.Add("QA");
                ServiceType.Items.Add("Live");
                LocationType.Items.Clear();
            }
            else
            {
                MessageBox.Show("File Load Failed : Config.json");
            }
        }

        private void UpdateBtn_Click(object sender, EventArgs e)
        {
            if (selectedData.IsEnbale())
            {
                jobj["ServerInfo"]["Maintenance"] = Check_Inspect.Checked;
                jobj["ServerInfo"]["Message"]["Msg"] = NoticeText.Text;
                jobj["ServerInfo"]["Message"]["Title"] = TitleText.Text;

                //File.WriteAllText(selectedData.filename, jobj.ToString());

                string ftpPath = "ftp://" + selectedData.ip + ":" + selectedData.port + "/" + selectedData.path + "/" + selectedData.filename;
                FtpWebRequest req = (FtpWebRequest)WebRequest.Create(new Uri(ftpPath));
                req.Method = WebRequestMethods.Ftp.UploadFile;
                req.Credentials = new NetworkCredential(selectedData.id, selectedData.pw);

                byte[] data;
                data = Encoding.UTF8.GetBytes(jobj.ToString());
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
                MessageBox.Show("Config information is missing or incorrect.");
            }
        }

        private void ServiceType_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateBtn.Enabled = false;
            LocationType.Items.Clear();
            TitleText.Clear();
            NoticeText.Clear();

            string val = ServiceType.SelectedItem as string;            
            if (val.Equals("QA", StringComparison.OrdinalIgnoreCase))
            {
                foreach (var data in loader.QAData)
                    LocationType.Items.Add(data.name);
            }
            else if (val.Equals("Live", StringComparison.OrdinalIgnoreCase))
            {
                foreach (var data in loader.LiveData)
                    LocationType.Items.Add(data.name);
            }
        }

        private void LocationType_SelectedIndexChanged(object sender, EventArgs e)
        {
            DownLoadFile();            
        }

        private void DownLoadFile()
        {
            UpdateBtn.Enabled = false;
            string loc = LocationType.SelectedItem as string;
            if(string.IsNullOrEmpty(loc))
            {
                MessageBox.Show("Please select a region first");
                return;
            }

            string type = ServiceType.SelectedItem as string;
            if (string.IsNullOrEmpty(type))
            {
                MessageBox.Show("Please select a Service Type");
                return;
            }

            selectedData = loader.GetData(type, loc);
            TitleText.Clear();
            NoticeText.Clear();
            if (selectedData.IsEnbale())
            {
                string ftpPath = "ftp://" + selectedData.ip + ":" + selectedData.port + "/" + selectedData.path + "/" + selectedData.filename;
                FtpWebRequest req = (FtpWebRequest)WebRequest.Create(new Uri(ftpPath));
                req.Method = WebRequestMethods.Ftp.DownloadFile;
                req.Credentials = new NetworkCredential(selectedData.id, selectedData.pw);
                using (FtpWebResponse resp = (FtpWebResponse)req.GetResponse())
                {
                    Stream stream = resp.GetResponseStream();

                    StreamReader reader = new StreamReader(stream);
                    string jsonValue = reader.ReadToEnd();
                    jobj = JObject.Parse(jsonValue);
                    
                    string check = jobj["ServerInfo"]["Maintenance"].ToString();
                    bool value = check.Equals("TRUE", StringComparison.OrdinalIgnoreCase);
                    Check_Inspect.Checked = value;

                    TitleText.Text = jobj["ServerInfo"]["Message"]["Title"].ToString();
                    NoticeText.Text = jobj["ServerInfo"]["Message"]["Msg"].ToString();

                    UpdateBtn.Enabled = true;
                }
            }
            else
            {
                MessageBox.Show("Config information is missing or incorrect.");
            }
        }

        private void ReloadBtn_Click(object sender, EventArgs e)
        {
            DownLoadFile();
        }
    }
}
