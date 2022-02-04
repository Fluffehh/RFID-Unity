using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;

public class RFIDTestServer : MonoBehaviour
{
	#region private members 	
	/// <summary> 	
	/// TCPListener to listen for incomming TCP connection 	
	/// requests. 	
	/// </summary> 	
	private TcpListener tcpListener;
	/// <summary> 
	/// Background thread for TcpServer workload. 	
	/// </summary> 	
	private Thread tcpListenerThread;
	/// <summary> 	
	/// Create handle to connected tcp client. 	
	/// </summary> 	
	private TcpClient connectedTcpClient;
	#endregion

	public string ClientMessage { get; set; }
	private string newMessage;
	private string csvPath = @"your CSV file path";
	private string textWritePath = @"your text file path to be rewrite";

	// Use this for initialization
	void Start()
	{
		// Start TcpServer background thread 		
		tcpListenerThread = new Thread(new ThreadStart(ListenForIncommingRequests));
		tcpListenerThread.IsBackground = true;
		tcpListenerThread.Start();
	}
    void Update()
    {
		if (newMessage != ClientMessage)
		{
			string CSVdata = CSVReader.GetCSV(ClientMessage, csvPath);
			WriteText.WritingText(CSVdata, textWritePath);
			Debug.Log(CSVdata);
			newMessage = ClientMessage;
		}
    }
    private void ListenForIncommingRequests()
	{
		string getIp = LocalIpAddress.LocalIPAddress();
		try
		{
			// Create listener on localhost port 8052. 			
			tcpListener = new TcpListener(IPAddress.Parse(getIp), 8052);
			tcpListener.Start();
			Debug.Log($"Server is listening : {getIp}");
			Byte[] bytes = new Byte[1024];
			while (true)
			{
				using (connectedTcpClient = tcpListener.AcceptTcpClient())
				{
					// Get a stream object for reading 					
					using (NetworkStream stream = connectedTcpClient.GetStream())
					{
						int length;
						// Read incomming stream into byte arrary. 						
						while ((length = stream.Read(bytes, 0, bytes.Length)) != 0)
						{
							var incommingData = new byte[length];
							Array.Copy(bytes, 0, incommingData, 0, length);
							// Convert byte array to string message. 							
							ClientMessage = Encoding.ASCII.GetString(incommingData);
							Debug.Log("client message received as: " + ClientMessage);
						}
					}
				}
			}
		}
		catch (SocketException socketException)
		{
			Debug.Log("SocketException " + socketException.ToString());
		}
	}
}