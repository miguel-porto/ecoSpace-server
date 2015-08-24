package ecoSpace;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class EcoSpace {
	public static void main( final String[] args ) throws InterruptedException {
		int maxAttemps=100;
		StartServer();
		if(args.length<1) {
			System.out.println("Expected arguments start | stop | direct");
			return;
		}
		
		if(args[0].equals("direct")) {
			System.out.println("Starting server directly. Press Ctrl+C to quit.");
			StartServer();
		}
		if(args[0].equals("start")) {
			System.out.println("Starting server on port 7530");
			System.out.print("\nWaiting for server to be ready");
			try {
				Runtime.getRuntime().exec("java -Djava.library.path="+System.getProperty("user.dir")+" -jar ecoSpace.jar direct");
			} catch (IOException e) {
				System.out.println(e.getMessage());
				return;
			}
			//java -Djava.library.path=/home/miguel/workspace/ecoSpace/jni -jar
			Socket server=null;
			
		    int attempts = 0;
	        while(attempts < maxAttemps) {
				try {
					server = new Socket("localhost", 7530);
				} catch (IOException e) {
					System.out.print(".");
				}
				if(server!=null) break;
	            attempts++;
	            Thread.sleep(1000);
	        }
	        if(attempts==maxAttemps)
	        	System.out.println("\nServer not ready...");
	        else
	        	System.out.println("\nServer ready on port 7530");
	        
		}
		
		if(args[0].equals("stop")) {
			System.out.println("Opening socket...");
			Socket server;
			PrintWriter out;
			InputStream instream;
			try {
				server = new Socket("localhost", 7530);
				out = new PrintWriter(server.getOutputStream(), true);
				 instream=server.getInputStream();
			} catch(IOException e) {
				System.out.println(e.getMessage());
				return;
			}
		    
		    // send stop message to server
		    out.println("stop");
		    
		    System.out.println("Waiting for server to stop");
		    BufferedReader in = new BufferedReader(new InputStreamReader(instream));
		    String a;
		    int attempts = 0;
		    
		    try {
		        while(attempts < maxAttemps) {
		        	a=in.readLine();
		        	if(a==null)
		        		System.out.print(".");
		        	else if(a.equals("STOPPED")) {
		        		System.out.println(a);
		        		break;
		        	} else
		        		System.out.println(a);
		            attempts++;
		            Thread.sleep(1000);
		        }
		        out.close();
		        in.close();
		        server.close();
		    } catch(IOException e) {
				System.out.println(e.getMessage());
				return;		    	
		    }
		}
			
		/*
to convert WorldClim to TIF
for f in *.bil; do gdal_translate -of GTiff -co "TFW=YES" -outsize 50% 50% $f ${f%.*}.tif; done
*/
		//String key=requestData(6635,"POLYGON((-9.78 44.11,-10.17 35.71,4.56 35.08,4.67 44.17,-9.78 44.11))");
		//String key="0009287-141123120432318";
		//String file="/tmp/009287-141123120432318.zi";
		/*String file=waitForDownload(key);
		simpleDataset ds=unzipAndReadOccurrences(file);*/

/*		File fi=Files.createFile(Paths.get("/home/miguel/exp.txt")).toFile();
		BufferedWriter bw=new BufferedWriter(new FileWriter(fi));
		for(simpleDataset.record r:ds.records) {
			bw.write(r.speciesName+"\n");
		}
		bw.close();*/
	}
	public static void StartServer() {
		new DatasetIndex();
		DatasetServer dss = new DatasetServer();
		MultiThreadedServer server = new MultiThreadedServer(7530,dss);
		server.run();
	}
}
