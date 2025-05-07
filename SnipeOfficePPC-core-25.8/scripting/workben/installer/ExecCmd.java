package installer;
import java.util.*;
import java.io.*;
public class ExecCmd
{

    public boolean exec( String cmd, String[] env )
    {
       System.out.println("About to exectute " + cmd);
       final Process p;
       boolean result = false;
       try
       {
           Runtime rt = Runtime.getRuntime();
           p=rt.exec( cmd, env );
           new Thread(new Runnable() {
               public void run()
               {
                   BufferedReader br_in = null;
                   try
                   {
                       br_in = new BufferedReader(new InputStreamReader(p.getInputStream()));
                       String buff = null;
                       while ((buff = br_in.readLine()) != null)
                       {
                           System.out.println("Process out :" + buff);
                           /*try
                           {
                               Thread.sleep(100);
                           }
                           catch(Exception e) {}*/
                       }
                       System.out.println("finished reading out");
                    }
                    catch (IOException ioe)
                    {
                        System.out.println("Exception caught printing javac result");
                        ioe.printStackTrace();
                    }
                    finally
                    {
                       if ( br_in != null )
                       {
                           try
                           {
                               br_in.close();
                           }
                           catch( Exception e ) {} // nothing can be done
                       }
                    }
               } } ).start();

            new Thread(new Runnable() {
                public void run() {
                BufferedReader br_err = null;
                try {
                    br_err = new BufferedReader(new InputStreamReader(p.getErrorStream()));
                    String buff = null;
                    while ((buff = br_err.readLine()) != null) {
                    System.out.println("Process err :" + buff);
                    /*try {Thread.sleep(100); } catch(Exception e) {}*/
                 }
                 System.out.println("finished reading err");
                 } catch (IOException ioe) {
                    System.out.println("Exception caught printing javac result");
                    ioe.printStackTrace();
                 }
                 finally
                 {
                    if ( br_err != null )
                    {
                        try
                        {
                            br_err.close();
                        }
                        catch( Exception e ) {} // nothing can be done
                    }
                 }
            } }).start();
            int exitcode = p.waitFor();
            if ( exitcode != 0 )
            {
                System.out.println("cmd [" + cmd + "] failed" );
                result= false;
            }
            else
            {
                System.out.println("cmd [" + cmd + "] completed successfully");
                result= true;
            }
        }
        catch (Exception e) {
          System.out.println("Exception");
          e.printStackTrace();
       }
       System.out.println("command complete");
       return result;
    }
}

