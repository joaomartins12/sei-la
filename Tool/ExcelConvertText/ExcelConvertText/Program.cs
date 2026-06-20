using System;
using System.Collections.Generic;

namespace ExcelConvertText
{
    class Program
    {
        static void Main(string[] args)
        {
            string readConfigFile = "ExcelToString_Config.txt";
            if (args.Length == 1)
                readConfigFile = args[0];
            Confige parser = new Confige();
            if( !parser.CheckConfigFile(readConfigFile) )
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("{0} 파일이 없습니다.", readConfigFile);
                Console.WriteLine("계속 하시려면 아무 키나 누르세요.");
                Console.ReadKey();
                return;
            }

            var maps = new Dictionary<string,List<Confige.sInfos>>();
            if( !parser.GetConfigeInfo(maps) )
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("{0} 파일의 형식이 잘못 됬습니다.", readConfigFile);
                Console.WriteLine("계속 하시려면 아무 키나 누르세요.");
                Console.ReadKey();
                return;
            }

            if (0 == maps.Count)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("{0} 파일에 변환할 데이터가 없습니다.", readConfigFile);
                Console.WriteLine("계속 하시려면 아무 키나 누르세요.");
                Console.ReadKey();
                return;
            }                

            Converter excel = new Converter();
            try
            {
                string path = System.IO.Directory.GetCurrentDirectory() + "\\";
                excel.Convert(path, maps);
            }
            catch
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("파일 변환에 실패 하였습니다.");
                excel.Distroy();
            }
            finally {
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }

           
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("계속 하시려면 아무 키나 누르세요.");
            Console.ReadKey();
        }  
    }
}
