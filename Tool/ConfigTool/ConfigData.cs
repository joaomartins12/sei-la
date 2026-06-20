using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json.Linq;

namespace ConfigTool
{
    class ConfigData
    {
        public struct locationData
        {
            public string name;
            public string ip;
            public string port;
            public string id;
            public string pw;
            public string path;
            public string filename;

            public bool IsEnbale()
            {
                if ("" == name)
                    return false;
                if ("" == ip)
                    return false;
                if ("" == port)
                    return false;
                if ("" == pw)
                    return false;
                if ("" == name)
                    return false;
                if ("" == path)
                    return false;
                if ("" == filename)
                    return false;
                return true;
            }
        }

        private List<locationData> mQaData;
        private List<locationData> mLiveData;

        public bool LoadConfigFile(string s)
        {
            string value = File.ReadAllText(s);
            if (value == "")
                return false;

            mQaData = new List<locationData>();
            JObject root = JObject.Parse(value);

            foreach (JObject obj in root["QA"])
            {
                locationData data = new locationData();
                data.name = obj["Location"].ToString();
                data.ip = obj["IP"].ToString();
                data.port = obj["port"].ToString();
                data.id = obj["id"].ToString();
                data.pw = obj["pw"].ToString();
                data.path = obj["path"].ToString();
                data.filename = obj["filename"].ToString();
                mQaData.Add(data);
            }

            mLiveData = new List<locationData>();
            foreach (JObject obj in root["Live"])
            {
                locationData data = new locationData();
                data.name = obj["Location"].ToString();
                data.ip = obj["IP"].ToString();
                data.port = obj["port"].ToString();
                data.id = obj["id"].ToString();
                data.pw = obj["pw"].ToString();
                data.path = obj["path"].ToString();
                data.filename = obj["filename"].ToString();
                mLiveData.Add(data);
            }

            return true;
        }

        public List<locationData> QAData
        {
            get
            {
                return mQaData;
            }
        }

        public List<locationData> LiveData
        {
            get
            {
                return mLiveData;
            }
        }

        public locationData GetData(string type, string loc)
        {
            if( "QA" == type )
            {
                foreach( var vv in mQaData )
                {
                    if (vv.name == loc)
                        return vv;
                }
            }

            if ("Live" == type)
            {
                foreach (var vv in mLiveData)
                {
                    if (vv.name == loc)
                        return vv;
                }
            }

            return new locationData();
        }
    }
}
