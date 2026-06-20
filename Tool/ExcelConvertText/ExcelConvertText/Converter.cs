using System;
using System.IO;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using Excel = Microsoft.Office.Interop.Excel;

namespace ExcelConvertText
{
    class Converter
    {
        public Converter()
        {
        }
        ~Converter()
        {
        }

        public void Distroy()
        {
        }
        
        public bool Convert(string path, Dictionary<string, List<Confige.sInfos>> info)
        {
            Excel.Application ExcelApp = new Excel.Application();
            CLIDmCS.Compress cliCom = new CLIDmCS.Compress();
            Excel.Workbook wkb = null;
            Excel.Worksheet wks = null;

            int nIndex = -1;
            foreach (var readData in info)
            {
                Console.ForegroundColor = ConsoleColor.Gray;
                try
                {
                    Console.Write("파일 읽기 : " + readData.Key);
                    wkb = ExcelApp.Workbooks.Open(path + readData.Key, 0, true, 5, "", "", true, Excel.XlPlatform.xlWindows, "\t", false, false, 0, true, 1, 0);
                    Console.Write("...OK");
                }
                catch
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Write("...FALSE");
                    Console.WriteLine();
                    continue;
                }

                Console.WriteLine();
                foreach (var SheetInfo in readData.Value)
                {
                    Console.ForegroundColor = ConsoleColor.Gray;
                    nIndex++;
                    try
                    {
                        Console.Write("[" + nIndex + "]" + "...Sheet 읽기 : " + SheetInfo.readSheetName);
                        wks = wkb.Worksheets.Item[SheetInfo.readSheetName];
                        Console.Write("...OK");
                    }
                    catch
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.Write("...FALSE");
                        Console.WriteLine();
                        continue;
                    }
                                        
                    Console.Write("....파일 읽는중: " + SheetInfo.saveFileName);
                    int nRowcount = wks.UsedRange.Rows.Count;
                    List<string> totalString = new List<string>();
                    var usedRangeValue2 = wks.UsedRange.Value2;
                    for (int rowCounter = SheetInfo.nStartIdx; rowCounter <= nRowcount; ++rowCounter)
                    {
                        string valueString = "";
                        if (0 == SheetInfo.readColNums.Count)
                        {
                            int nColcount = wks.UsedRange.Columns.Count;
                            for (int colCounter = 1; colCounter < nColcount; ++colCounter)
                            {
                                var cell = usedRangeValue2[rowCounter, colCounter];
                                if (cell == null)
                                    valueString = string.Concat(valueString, "|");
                                else
                                    valueString = string.Concat(valueString, cell.ToString() + "|");
                            }
                        }
                        else
                        {
                            foreach (int colCounter in SheetInfo.readColNums)
                            {
                                var cell = usedRangeValue2[rowCounter, colCounter];
                                if (cell == null)
                                    valueString = string.Concat(valueString, "|");
                                else
                                    valueString = string.Concat(valueString, cell.ToString() + "|");
                            }
                        }

                        valueString = Regex.Replace(valueString, @"\r\n", "†", RegexOptions.Singleline);
                        totalString.Add(valueString.Substring(0, valueString.Length - 1));
                    }

                    if ( 0 != totalString.Count )
                    {
                        Console.Write("...OK");
                        Console.Write("...파일 저장중 ");

                        if( SheetInfo.bSaveBinary )
                        {
                            Byte[] result = cliCom.CompressData(totalString);
                            SaveBinaryFile(SheetInfo.saveFileName, result);
                        }
                        else
                        {
                            SaveUTF8File(SheetInfo.saveFileName, totalString);
                        }

                        Console.Write("...OK");
                        Console.WriteLine();                        
                    }                 
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.Write(@"...FALSE");
                        Console.WriteLine();
                    }
                }
                DeleteObject(wks);
                wkb.Close(false, System.Reflection.Missing.Value, System.Reflection.Missing.Value);
                DeleteObject(wkb);
            }

            ExcelApp.Quit();
            DeleteObject(ExcelApp);
            GC.Collect();
            GC.WaitForPendingFinalizers();
            return true;
        }
        private void DeleteObject(object obj)
        {
            if (obj == null)
                return;
            try
            {
                System.Runtime.InteropServices.Marshal.FinalReleaseComObject(obj);
                obj = null;
            }
            catch (Exception ex)
            {
                obj = null;
                Console.WriteLine("메모리 할당을 해제하는 중 문제 발생" + ex.ToString(), "경고");
            }
            finally
            {
                GC.Collect();
            }
        }

        private void SaveUTF8File(string saveName, List<string> saveData )
        {
            Stream FS = new FileStream(saveName, FileMode.Create, FileAccess.Write);
            using (StreamWriter sw = new StreamWriter(FS, System.Text.Encoding.UTF8))
            {
                foreach (String s in saveData)
                    sw.WriteLine(s);
            }
            FS.Close();
        }

        private void SaveBinaryFile(string saveName, byte[] saveData)
        {
            FileStream FS = new FileStream(saveName, FileMode.Create, FileAccess.Write);
            using (BinaryWriter wr = new BinaryWriter(FS))
            {
                wr.Write(saveData, 0, saveData.Length);
            }
            FS.Close();           
        }


    }
}
