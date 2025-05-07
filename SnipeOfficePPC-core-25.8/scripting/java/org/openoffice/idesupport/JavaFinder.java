package org.openoffice.idesupport;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Vector;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.net.URL;
import java.net.URLClassLoader;
import java.net.MalformedURLException;
import org.openoffice.idesupport.zip.ParcelZipper;

import com.sun.star.script.framework.container.ScriptEntry;

public class JavaFinder implements MethodFinder {

    private static JavaFinder finder;
    private static final String JAVA_SUFFIX = ".java";
    private static final String CLASS_SUFFIX = ".class";
    private static final String LANGUAGE = "Java";
    private static final String FIRST_PARAM =
        "drafts.com.sun.star.script.framework.runtime.XScriptContext";

    private Vector classpath = null;

    private JavaFinder() {}

    private JavaFinder(Vector classpath) {
        this.classpath = classpath;
    }

    public static JavaFinder getInstance() {
        if (finder == null) {
            synchronized(JavaFinder.class) {
                if (finder == null)
                    finder = new JavaFinder();
            }
        }
        return finder;
    }

    public static JavaFinder getInstance(Vector classpath) {
        return new JavaFinder(classpath);
    }

    public ScriptEntry[] findMethods(File basedir) {
        String parcelName;
        ArrayList result = new ArrayList(10);
        ScriptEntry[] empty = new ScriptEntry[0];

        if (basedir == null || basedir.exists() == false ||
            basedir.isDirectory() == false)
            return empty;

        parcelName = basedir.getName();
        if (parcelName.equals(ParcelZipper.CONTENTS_DIRNAME))
            parcelName = basedir.getParentFile().getName();

        String[] classNames = findClassNames(basedir);
        if (classNames != null && classNames.length != 0) {

            ClassLoader classloader;

            if (classpath == null)
                classloader = getClassLoader(basedir);
            else
                classloader = getClassLoader();

            for (int i = 0; i < classNames.length; i++)
            {
                try
                {
                    Class clazz = classloader.loadClass(classNames[i]);
                    Method[] methods = clazz.getDeclaredMethods();
                    for (int k = 0; k < methods.length; k++)
                    {
                        if (Modifier.isPublic(methods[k].getModifiers()))
                        {
                            Class[] params = methods[k].getParameterTypes();
                            if(params.length > 0)
                            {
                                if(params[0].getName().equals(FIRST_PARAM))
                                {
                                    ScriptEntry entry =
                                        new ScriptEntry(classNames[i] + "." +
                                            methods[k].getName(), parcelName);
                                    result.add(entry);
                                }
                            }
                        }
                    }
                }
                catch (ClassNotFoundException e)
                {
                    System.err.println("Caught ClassNotFoundException loading: "
                        + classNames[i]);
                    continue;
                }
                catch (NoClassDefFoundError nc)
                {
                    System.err.println("Caught NoClassDefFoundErr loading: " +
                        classNames[i]);
                    continue;
                }
            }
        }

        if (result.size() != 0)
            return (ScriptEntry[])result.toArray(empty);
        return empty;
    }

    private ClassLoader getClassLoader() {
        int len = classpath.size();
        ArrayList urls = new ArrayList(len);

        for (int i = 0; i < len; i++) {
            try {
                String s = (String)classpath.elementAt(i);
                s = SVersionRCFile.toFileURL(s);

                if (s != null)
                    urls.add(new URL(s));
            }
            catch (MalformedURLException mue) {
            }
        }

        return new URLClassLoader((URL[])urls.toArray(new URL[0]));
    }

    private ClassLoader getClassLoader(File basedir) {
        ArrayList files = findFiles(basedir, ".jar");
        files.add(basedir);

        try {
            Enumeration offices = SVersionRCFile.createInstance().getVersions();

            while (offices.hasMoreElements()) {
                OfficeInstallation oi = (OfficeInstallation)offices.nextElement();
                String unoil = SVersionRCFile.getPathForUnoil(oi.getPath());

                if (unoil != null) {
                    files.add(new File(unoil, "unoil.jar"));
                    break;
                }
            }
        }
        catch (IOException ioe) {
            return null;
        }

        URL[] urls = new URL[files.size()];
        String urlpath;
        File f;
        for (int i = 0; i < urls.length; i++) {
            try {
                f = (File)files.get(i);
                urlpath = SVersionRCFile.toFileURL(f.getAbsolutePath());

                if (urlpath != null)
                    urls[i] = new URL(urlpath);
            }
            catch (MalformedURLException mue) {
                // do nothing, go on to next file
            }
        }

        return new URLClassLoader(urls);
    }

    private ArrayList findFiles(File basedir, String suffix) {
        ArrayList result = new ArrayList();
        File[] children = basedir.listFiles();

        for (int i = 0; i < children.length; i++) {
            if (children[i].isDirectory())
                result.addAll(findFiles(children[i], suffix));
            else if (children[i].getName().endsWith(suffix))
                result.add(children[i]);
        }
        return result;
    }

    private String[] findClassNames(File basedir)
    {
        ArrayList classFiles = findFiles(basedir, CLASS_SUFFIX);
        if(classFiles == null || classFiles.size() == 0)
            return null;

        ArrayList javaFiles = findFiles(basedir, JAVA_SUFFIX);
        if(javaFiles == null || javaFiles.size() == 0)
            return null;

        ArrayList result = new ArrayList();
        for (int i = 0; i < classFiles.size(); i++)
        {
            File classFile = (File)classFiles.get(i);
            String className = classFile.getName();
            className = className.substring(0, className.lastIndexOf(CLASS_SUFFIX));
            boolean finished = false;


            for (int j = 0; j < javaFiles.size() && finished == false; j++)
            {
                File javaFile = (File)javaFiles.get(j);
                String javaName = javaFile.getName();
                javaName = javaName.substring(0, javaName.lastIndexOf(JAVA_SUFFIX));

                if (javaName.equals(className))
                {
                    String path = classFile.getAbsolutePath();
                    path = path.substring(basedir.getAbsolutePath().length() + 1);
                    path = path.replace(File.separatorChar, '.');
                    path = path.substring(0, path.lastIndexOf(CLASS_SUFFIX));

                    result.add(path);
                    javaFiles.remove(j);
                    finished = true;
                }
            }
        }
        return (String[])result.toArray(new String[0]);
    }
}
