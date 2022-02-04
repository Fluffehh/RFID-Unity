using System.IO;
using UnityEngine;

public static class CSVReader
{
    public static string GetCSV(string message, string path)
    {
        StreamReader strReader = new StreamReader(path);

        while (!strReader.EndOfStream)
        {
            var line = strReader.ReadLine();
            var values = line.Split(',');

            for (int i = 0; i < values.Length; i++)
            {
                if (message == values[0])
                {
                    return values[1].ToString();
                }

            }

        }
        return "Nothing Found";
    }
}

