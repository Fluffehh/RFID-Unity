using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

public static class WriteText
{
    public static void WritingText(string csvData, string filePath)
    {
        File.WriteAllText(filePath, csvData);
    }
}

