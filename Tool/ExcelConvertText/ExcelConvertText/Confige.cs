using System;
using System.IO;
using System.Collections.Generic;

namespace ExcelConvertText
{    class Confige
    {
        public struct sInfos
        {
            public int nStartIdx;           // 행의 시작 위치 인덱스값
            public string readSheetName;    // 읽을 시트명
            public string saveFileName;     // 저장할 파일 명
            public bool bSaveBinary;        // 바이러리 저장 
            public List<int> readColNums;   // 읽을 컬럼 인덱스 1번 부터 시작임.
        }
        
        private string ConfigeFile;
        public bool CheckConfigFile( string file )
        {
            if (!File.Exists(file))
                return false;

            ConfigeFile = file;
            return true;
        }

        public bool GetConfigeInfo( Dictionary<string, List<sInfos>> info )
        {
            string[] lines = File.ReadAllLines(ConfigeFile);
            if (0 == lines.Length)
                return false;

            foreach (var line in lines)
            {
                if (string.IsNullOrEmpty(line) || line[0].ToString() == ";")
                    continue;

                string[] tokens = line.Split(' ');
                if (tokens.Length < 4)
                    continue;

                List<sInfos> getlists;
                if (!info.TryGetValue(tokens[0], out getlists))
                {
                    getlists = new List<sInfos>();
                    info.Add(tokens[0], getlists);
                }

                sInfos addItem = new sInfos();
                addItem.readSheetName = tokens[1];
                addItem.saveFileName = tokens[2];
                string exeName = Path.GetExtension(addItem.saveFileName);
                addItem.bSaveBinary = exeName.Equals(".BIN", StringComparison.OrdinalIgnoreCase);
                addItem.nStartIdx = Int32.Parse(tokens[3]);
                addItem.readColNums = new List<int>();

                if (addItem.nStartIdx < 1)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("읽을 행의 시작 인덱스는 1보다 작으면 안됩니다.");
                    return false;
                }

                if (tokens.Length >= 5)
                {
                    string colNum = tokens[4];
                    if (!string.IsNullOrWhiteSpace(colNum) && !string.IsNullOrEmpty(colNum))
                    {
                        string[] tok = colNum.Split(',');
                        foreach (string val in tok)
                        {
                            int nReadColNum = Int32.Parse(val);
                            if(nReadColNum<1)
                            {
                                Console.WriteLine("읽을 컬럼의 인덱스는 1보다 작으면 안됩니다.");
                                return false;
                            }
                            addItem.readColNums.Add(nReadColNum);
                        }
                            
                    }
                }

                getlists.Add(addItem);
            }

            return true;
        }
    }
}
