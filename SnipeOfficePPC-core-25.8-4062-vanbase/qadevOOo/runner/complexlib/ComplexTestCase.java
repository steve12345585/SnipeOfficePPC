/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
package complexlib;

import java.lang.reflect.Method;
import share.DescEntry;
import lib.TestParameters;
import lib.StatusException;
import share.LogWriter;
import share.ComplexTest;
import java.io.PrintWriter;

/**
 * Base class for all complex tests.
 */
public abstract class ComplexTestCase extends Assurance implements ComplexTest
{

    /** The test parameters **/
    protected static TestParameters param = null;
    /** Log writer **/
    protected static LogWriter log = null;
    /**
     * The method name which will be written into f.e. the data base
     **/
    protected String mTestMethodName = null;
    /** Maximal time one method is allowed to execute
     * Can be set with parameter 'ThreadTimeOut'
     **/
    protected int m_nThreadTimeOut = 0;
    /** Continue a test even if it did fail **/
    // public static final boolean CONTINUE = true;

    /** End a test if it did fail **/
    public static final boolean BREAK = true;

    private boolean m_bBeforeCalled;

    /**
     * is called before the real test starts
     */
    private void before()
    {
        try
        {
            Method before = this.getClass().getMethod("before", new Class[] {} );
            before.invoke(this, new Object[] {} );

            // beforeWorked = false;
            m_bBeforeCalled = true;
        }
        catch (java.lang.NoSuchMethodException e)
        {
            m_bBeforeCalled = true;
        }
        catch (java.lang.IllegalAccessException e)
        {
            log.println("Cannot access the 'before()' method, although it" + " is there. Is this ok?");
        }
        catch (java.lang.reflect.InvocationTargetException e)
        {
            Throwable t = e.getTargetException();
            if (!(t instanceof RuntimeException) || state)
            {
                log.println(t.toString());
                if (message == null)
                {
                    message = "Exception in before() method.\n\r" + t.getMessage();
                }
                state = false;
                t.printStackTrace((PrintWriter) log);
            }
        }

    }

    /** Description entry **/
    // protected DescEntry subEntry = null;

    private void test_method(DescEntry _entry)
    {

        m_nThreadTimeOut = param.getInt("ThreadTimeOut");
        if (m_nThreadTimeOut == 0)
        {
            m_nThreadTimeOut = 300000;
        }

        for (int i = 0; i < _entry.SubEntries.length; i++)
        {

            DescEntry subEntry = _entry.SubEntries[i];
            if (m_bBeforeCalled)
            {
                state = true;
                message = "";
            }
            else
            {
                // set all test methods on failed, if 'before()' did not work.
                subEntry.State = message;
                subEntry.hasErrorMsg = true;
                subEntry.ErrorMsg = message;
                continue;
            }
            Method testMethod = null;
            try
            {
                String entryName = subEntry.entryName;
                Object[] parameter = null;

                if (entryName.indexOf("(") != -1)
                {
                    String sParameter = (entryName.substring(entryName.indexOf("(") + 1, entryName.indexOf(")")));
                    mTestMethodName = entryName;
                    parameter = new String[] { sParameter };
                    entryName = entryName.substring(0, entryName.indexOf("("));
                    testMethod = this.getClass().getMethod(entryName, new Class[] { String.class });
                }
                else
                {
                    testMethod = this.getClass().getMethod(entryName, new Class[] {} );
                    mTestMethodName = entryName;
                }

                MethodThread th = new MethodThread(testMethod, this, parameter, (java.io.PrintWriter) log);
                log.println("Starting " + mTestMethodName);
                th.start();

                try
                {
                    // some tests are very dynamic in its exceution time so that
                    // a threadTimeOut fials. In this cases the logging mechanisim
                    // is a usefull way to detect that a office respective a test
                    // is running and not death.
                    // But way ThreadTimeOut?
                    // There exeitsts a complex test which uses no office. Therefore
                    // a logging mechanisim to detect a stalled test.
                    int lastPing = -1;
                    int newPing = 0;

                    int sleepingStep = 1000;
                    int factor = 0;

                    while (th.isAlive() && (lastPing != newPing || factor * sleepingStep < m_nThreadTimeOut))
                    {
                        Thread.sleep(sleepingStep);
                        factor++;
                        // if a test starts the office itself it the watcher is a
                        // new one.
                        share.Watcher ow = (share.Watcher) param.get("Watcher");
                        if (ow != null)
                        {
                            lastPing = newPing;
                            newPing = ow.getPing();
                            //System.out.println("lastPing: '" + lastPing + "' newPing '" + newPing + "'");
                            factor = 0;
                        }
                    }
                }
                catch (InterruptedException e)
                {
                }
                if (th.isAlive())
                {
                    log.println("Destroy " + mTestMethodName);
                    th.stopRunning();
                    subEntry.State = "Test did sleep for " + (m_nThreadTimeOut / 1000) + " seconds and has been killed!";
                    subEntry.hasErrorMsg = true;
                    subEntry.ErrorMsg = subEntry.State;
                    continue;
                }
                else
                {
                    log.println("Finished " + mTestMethodName);
                    if (th.hasErrorMessage())
                    {
                        subEntry.State = th.getErrorMessage();
                        subEntry.hasErrorMsg = true;
                        subEntry.ErrorMsg = subEntry.State;
                        continue;
                    }
                }
            }
            catch (java.lang.Exception e)
            {
                log.println(e.getClass().getName());
                String msg = e.getMessage();
                log.println("Message: " + msg);
                e.printStackTrace((PrintWriter) log);
                subEntry.State = "SKIPPED.FAILED";
                subEntry.hasErrorMsg = true;
                subEntry.ErrorMsg = (msg == null ? "" : msg);
                continue;
            }
            subEntry.State = (state ? "PASSED.OK" : message);
            subEntry.hasErrorMsg = !state;
            subEntry.ErrorMsg = message;
        }
    }

    /**
     * after() is called after the test is done
     */
    private void after()
    {
        if (m_bBeforeCalled)
        {
            // the after() method
            try
            {
                Method after = this.getClass().getMethod("after", new Class[] {});
                after.invoke(this, new Object[] {} );
            }
            catch (java.lang.NoSuchMethodException e)
            {
                // simply ignore
            }
            catch (java.lang.IllegalAccessException e)
            {
                // simply ignore
            }
            catch (java.lang.reflect.InvocationTargetException e)
            {
                Throwable t = e.getTargetException();
                if (!(t instanceof StatusException))
                {
                    log.println(t.toString());
                    if (message == null)
                    {
                        message = "Exception in after() method.\n\r" + t.getMessage();
                    }
                    else
                    {
                        message += "Exception in \'after()\' method.\n\r" + t.getMessage();
                    }
                    log.println("Message: " + message);
                    t.printStackTrace((PrintWriter) log);
                }
            }
        }

    }



    /**
     * Call test. It is expected, that an environment is
     * given to this test.
     *
     * @param entry The name of the test method that should be called.
     * @param environment The environment for the test.
     */
    public void executeMethods(DescEntry entry, TestParameters environment)
    {
        m_bBeforeCalled = false;

        // get the environment
        param = environment;
        log = entry.Logger;


        // start with the before() method
        before();

        //executeMethodTests
        test_method(entry);

        // cleanup
        after();
    }


    /**
     * Implement this method in the Complex test.
     * @return All test method names.
     */
    public abstract String[] getTestMethodNames();

    /**
     * Return a name for the test or tested object.
     * Override to give an own name.
     * @return As default, the name of this class.
     */
    public String getTestObjectName()
    {
        return this.getClass().getName();
    }
}
