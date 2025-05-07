/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
package com.sun.star.report;

import java.io.IOException;

/**
 * This allows some simple job control. A job can be interrupted (or if it has
 * not yet been started, canceled), and the job status can be queried (scheduled,
 * running, finished).
 *
 * <table border="1">
 * <tr>
 * <th>JobStatus</th><th>running</th><th>finished</th>
 * </tr>
 * <tr>
 * <td>Scheduled</td><td>false</td><td>false</td>
 * </tr>
 * <tr>
 * <td>Running</td><td>true</td><td>false</td>
 * </tr>
 * <tr>
 * <td>Finished</td><td>false</td><td>true</td>
 * </tr>
 * </table>
 *
 * @author Thomas Morgner
 */
public interface ReportJob
{

    /**
     * Although we might want to run the job as soon as it has been
     * created, sometimes it is wiser to let the user add some listeners
     * first. If we execute at once, the user either has to deal with
     * threading code or wont receive any progress information in single
     * threaded environments.
     * @throws java.io.IOException
     * @throws ReportExecutionException
     */
    void execute()
            throws ReportExecutionException, IOException;

    /**
     * Interrupt the job.
     */
    void interrupt();

    /**
     * Queries the jobs execution status.
     *
     * @return true, if the job is currently running, false otherwise.
     */
    boolean isRunning();

    /**
     * Queries the jobs result status.
     *
     * @return true, if the job is finished (or has been interrupted), false
     * if the job waits for activation.
     */
    boolean isFinished();

    void addProgressIndicator(JobProgressIndicator indicator);

    void removeProgressIndicator(JobProgressIndicator indicator);
}
