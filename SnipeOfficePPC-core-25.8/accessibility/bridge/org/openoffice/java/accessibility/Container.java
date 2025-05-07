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

package org.openoffice.java.accessibility;

import javax.accessibility.AccessibleContext;
import javax.accessibility.AccessibleState;

import com.sun.star.uno.*;
import com.sun.star.accessibility.*;

public class Container extends java.awt.Container implements javax.accessibility.Accessible {

    protected XAccessible unoAccessible;
    protected XAccessibleContext unoAccessibleContext;
    protected XAccessibleComponent unoAccessibleComponent = null;

    protected javax.accessibility.AccessibleRole accessibleRole;
    protected javax.accessibility.AccessibleText accessibleText;
    protected boolean disposed = false;

    protected Container(javax.accessibility.AccessibleRole role,
        XAccessible xAccessible, XAccessibleContext xAccessibleContext) {
        accessibleRole = role;
        unoAccessible = xAccessible;
        unoAccessibleContext = xAccessibleContext;
        unoAccessibleComponent = (XAccessibleComponent)
            UnoRuntime.queryInterface(XAccessibleComponent.class,
            xAccessibleContext);

        // Add the event listener right away, because the global focus notification doesn't
        // work yet ..
        XAccessibleEventBroadcaster broadcaster = (XAccessibleEventBroadcaster)
            UnoRuntime.queryInterface(XAccessibleEventBroadcaster.class,
            unoAccessibleContext);
        if (broadcaster != null) {
            broadcaster.addEventListener(createEventListener());
        }
    }

    /**
    * Determines whether this <code>Container</code> is showing on screen.
    * This means that the component must be visible, and it must be in a
    * <code>container</code> that is visible and showing.
    * @see       #addNotify
    * @see       #removeNotify
    * @since JDK1.0
    */
    public boolean isShowing() {
        if (isVisible()) {
            java.awt.Container parent = getParent();
            return (parent == null) || parent.isShowing();
        }
        return false;
    }

    /**
    * Makes this <code>Container</code> displayable by connecting it to a
    * native screen resource.
    * This method is called internally by the toolkit and should
    * not be called directly by programs.
    * @see       #isDisplayable
    * @see       #removeNotify
    * @since JDK1.0
    */
    public void addNotify() {
    }

    /**
    * Makes this <code>Container</code> undisplayable by destroying it native
    * screen resource.
    * This method is called by the toolkit internally and should
    * not be called directly by programs.
    * @see       #isDisplayable
    * @see       #addNotify
    * @since JDK1.0
    */
    public void removeNotify() {
    }

    /*
     * Fake the java focus handling. This is necessary to keep OOo focus
     * in sync with the java focus. See java.awt.DefaultKeyboardFocusManager
     * for implementation details.
     **/

    /** Requests focus for this object */
    public void requestFocus() {
    }

    /** Requests focus for this object */
    public boolean requestFocus(boolean temporary) {
        // Must be a no-op to make focus handling work
        return true;
    }

    /** Requests the focus for this object in the containing window */
    public boolean requestFocusInWindow() {
        return requestFocusInWindow(false);
    }

    /** Requests the focus for this object in the containing window */
    protected boolean requestFocusInWindow(boolean temporary) {
        if (isFocusable() && isVisible()) {
            getEventQueue().postEvent(new java.awt.event.FocusEvent(this, java.awt.event.FocusEvent.FOCUS_GAINED, temporary));
            return true;
        }
        return false;
    }

    public Object[] getAccessibleComponents(Object[] targetSet) {
        try {
            java.util.ArrayList list = new java.util.ArrayList(targetSet.length);
            for (int i=0; i < targetSet.length; i++) {
                java.awt.Component c = AccessibleObjectFactory.getAccessibleComponent(
                    (XAccessible) UnoRuntime.queryInterface(XAccessible.class, targetSet[i]));
                if (c != null) {
                    list.add(c);
                }
            }
            list.trimToSize();
            return list.toArray();
        } catch (com.sun.star.uno.RuntimeException e) {
            return null;
        }
    }

    protected java.awt.EventQueue getEventQueue() {
        return java.awt.Toolkit.getDefaultToolkit().getSystemEventQueue();
    }

    protected class PropertyChangeBroadcaster implements Runnable {
        String propertyName;
        Object oldValue;
        Object newValue;

        public PropertyChangeBroadcaster(String name, Object param1, Object param2) {
            propertyName = name;
            oldValue = param1;
            newValue = param2;
        }

        public void run() {
            // Because this code is executed in the DispatchThread, it is better tocatch every
            // exception that might occur
            try {
                AccessibleContext ac = Container.this.accessibleContext;
                if (ac != null) {
                    ac.firePropertyChange(propertyName, oldValue, newValue);
                }
            } catch (java.lang.Exception e) {
                if (Build.DEBUG) {
                    System.err.println(e.getClass().getName() + " caught propagating " + propertyName + " event: " + e.getMessage());
                    e.printStackTrace();
                }
            }
        }
    }

    protected void firePropertyChange(String property, Object oldValue, Object newValue) {
        getEventQueue().invokeLater(new PropertyChangeBroadcaster(property, oldValue, newValue));
    }

    protected void fireStatePropertyChange(AccessibleState state, boolean set) {
        PropertyChangeBroadcaster broadcaster;
        if (set) {
            broadcaster = new PropertyChangeBroadcaster(
                AccessibleContext.ACCESSIBLE_STATE_PROPERTY,
                null, state);
        } else {
            broadcaster = new PropertyChangeBroadcaster(
                AccessibleContext.ACCESSIBLE_STATE_PROPERTY,
                state, null);
        }
        getEventQueue().invokeLater(broadcaster);
    }

    /**
    * Update the proxy objects appropriatly on property change events
    */
    protected class AccessibleContainerListener implements XAccessibleEventListener {

        protected AccessibleContainerListener() {
        }

        protected java.awt.EventQueue getEventQueue() {
            return java.awt.Toolkit.getDefaultToolkit().getSystemEventQueue();
        }

        protected void setComponentState(short state, boolean enable) {
            switch (state) {
                case AccessibleStateType.ACTIVE:
                    // Only frames should be active
                    break;
                case AccessibleStateType.ENABLED:
                    setEnabled(enable);
                    // Since we can't access awt.Componet.accessibleContext, we need to fire
                    // this event manually ..
                    fireStatePropertyChange(AccessibleState.ENABLED, enable);
                    break;
                case AccessibleStateType.FOCUSED:
                    getEventQueue().postEvent(new java.awt.event.FocusEvent(
                        Container.this, enable ?
                        java.awt.event.FocusEvent.FOCUS_GAINED :
                        java.awt.event.FocusEvent.FOCUS_LOST));
                    break;
                case AccessibleStateType.SELECTED:
                    fireStatePropertyChange(AccessibleState.SELECTED, enable);
                    break;
                case AccessibleStateType.SENSITIVE:
                    // This state equals ENABLED in OOo (but not in Gtk+) and does not exist in Java 1.5
                    break;
                case AccessibleStateType.SHOWING:
                case AccessibleStateType.VISIBLE:
                    setVisible(enable);
                    break;
                default:
                    if (Build.DEBUG) {
                        System.err.println(Container.this + "unexpected state change " + state);
                    }
                    break;
            }
        }
        /** Updates the accessible name and fires the appropriate PropertyChangedEvent */
        protected void handleNameChangedEvent(Object any) {
            try {
                // This causes the property change event to be fired in the VCL thread
                // context. If this causes problems, it has to be deligated to the java
                // dispatch thread ..
                if (accessibleContext != null) {
                    accessibleContext.setAccessibleName(AnyConverter.toString(any));
                }
            } catch (com.sun.star.lang.IllegalArgumentException e) {
            }
        }

        /** Updates the accessible description and fires the appropriate PropertyChangedEvent */
        protected void handleDescriptionChangedEvent(Object any) {
            try {
                // This causes the property change event to be fired in the VCL thread
                // context. If this causes problems, it has to be deligated to the java
                // dispatch thread ..
                if (accessibleContext != null) {
                    accessibleContext.setAccessibleDescription(AnyConverter.toString(any));
                }
            } catch (com.sun.star.lang.IllegalArgumentException e) {
            }
        }

        /** Updates the internal states and fires the appropriate PropertyChangedEvent */
        protected void handleStateChangedEvent(Object any1, Object any2) {
            try {
                if (AnyConverter.isShort(any1)) {
                    setComponentState(AnyConverter.toShort(any1), false);
                }

                if (AnyConverter.isShort(any2)) {
                    setComponentState(AnyConverter.toShort(any2), true);
                }
            } catch (com.sun.star.lang.IllegalArgumentException e) {
            }
        }

        /* This event is only necessary because some objects in the office don't know their parent
         *  and are therefor unable to revoke and re-insert themselves.
         */
        protected void handleAllChildrenChangedEvent() {
            javax.accessibility.Accessible parent = (javax.accessibility.Accessible) getParent();
            if (parent != null) {
                javax.accessibility.AccessibleContext parentAC = parent.getAccessibleContext();
                if (parentAC != null) {

                    parentAC.firePropertyChange(
                        javax.accessibility.AccessibleContext.ACCESSIBLE_CHILD_PROPERTY,
                        Container.this,
                        null);

                    AccessibleObjectFactory.clearContainer(Container.this);
                    AccessibleObjectFactory.populateContainer(Container.this, unoAccessibleContext);

                    parentAC.firePropertyChange(
                        javax.accessibility.AccessibleContext.ACCESSIBLE_CHILD_PROPERTY,
                        null,
                        Container.this);
                }
            }
        }

        /** Called by OpenOffice process to notify property changes */
        public void notifyEvent(AccessibleEventObject event) {

            if ( !disposed ) {

                switch (event.EventId) {
                    case AccessibleEventId.NAME_CHANGED:
                        // Set the accessible name for the corresponding context, which will fire a property
                        // change event itself
                        handleNameChangedEvent(event.NewValue);
                        break;
                    case AccessibleEventId.DESCRIPTION_CHANGED:
                        // Set the accessible description for the corresponding context, which will fire a property
                        // change event itself - so do not set propertyName !
                        handleDescriptionChangedEvent(event.NewValue);
                        break;
                    case AccessibleEventId.STATE_CHANGED:
                        // Update the internal state set and fire the appropriate PropertyChangedEvent
                        handleStateChangedEvent(event.OldValue, event.NewValue);
                        break;
                    case AccessibleEventId.TEXT_CHANGED:
                        firePropertyChange(AccessibleContext.ACCESSIBLE_TEXT_PROPERTY,
                                                AccessibleTextImpl.convertTextSegment(event.OldValue),
                                                AccessibleTextImpl.convertTextSegment(event.NewValue));
                        break;
                    case AccessibleEventId.CHILD:
                        if (AnyConverter.isObject(event.OldValue)) {
                            AccessibleObjectFactory.removeChild(Container.this, event.OldValue);
                        } else if (AnyConverter.isObject(event.NewValue)) {
                            AccessibleObjectFactory.addChild(Container.this, event.NewValue);
                        }
                        break;
                    case AccessibleEventId.VISIBLE_DATA_CHANGED:
                    case AccessibleEventId.BOUNDRECT_CHANGED:
                        firePropertyChange(AccessibleContext.ACCESSIBLE_VISIBLE_DATA_PROPERTY, null, null);
                        break;
                    /*
                     * the Java AccessBridge for GNOME maps SELECTION_PROPERTY change events
                     * for objects of role TEXT to object:text-selection-changed
                     */
                    case AccessibleEventId.TEXT_SELECTION_CHANGED:
                    case AccessibleEventId.SELECTION_CHANGED:
                        firePropertyChange(AccessibleContext.ACCESSIBLE_SELECTION_PROPERTY, null, null);
                        break;
                    case AccessibleEventId.INVALIDATE_ALL_CHILDREN:
                        handleAllChildrenChangedEvent();
                        break;
                    default:
                        // Warn about unhandled events
                        if(Build.DEBUG) {
                            System.out.println(this + ": unhandled accessibility event id=" + event.EventId);
                        }
                }
            }
        }

        /** Called by OpenOffice process to notify that the UNO component is disposing */
        public void disposing(com.sun.star.lang.EventObject eventObject) {
            disposed = true;
            AccessibleObjectFactory.disposing(Container.this);
        }
    }

    protected XAccessibleEventListener createEventListener() {
        return new AccessibleContainerListener();
    }

    protected javax.accessibility.AccessibleContext accessibleContext = null;

    /** This method actually creates the AccessibleContext object returned by
     * getAccessibleContext().
     */
    protected javax.accessibility.AccessibleContext createAccessibleContext() {
        return new AccessibleContainer();
    }

    /** Returns the AccessibleContext associated with this object */
    public final javax.accessibility.AccessibleContext getAccessibleContext() {
        if (accessibleContext == null) {
            try {
                AccessibleContext ac = createAccessibleContext();
                if (ac != null) {
                    // Set accessible name and description here to avoid
                    // unnecessary property change events later ..
                    ac.setAccessibleName(unoAccessibleContext.getAccessibleName());
                    ac.setAccessibleDescription(unoAccessibleContext.getAccessibleDescription());
                    accessibleContext = ac;
                }
            } catch (com.sun.star.uno.RuntimeException e) {
            }
        }
        return accessibleContext;
    }

    protected class AccessibleContainer extends java.awt.Container.AccessibleAWTContainer {

        protected AccessibleContainer() {
            /* Since getAccessibleText() is heavily used by the java access
             * bridge for gnome and the gnome at-tools, we do a query interface
             * here and remember the result.
             */
            accessibleText = AccessibleTextImpl.get(unoAccessibleContext);
        }

        protected AccessibleContainer(boolean query) {
            /* This constructor is explicitly for subclasses that implement
             * AccessibleHypertext and therefor the default constructor would
             * bring unnecessary overhead.
             */
        }

        protected java.awt.event.ComponentListener accessibleComponentHandler = null;

        /**
        * Fire PropertyChange listener, if one is registered,
        * when shown/hidden..
        */
        protected class AccessibleComponentHandler implements java.awt.event.ComponentListener {
            public void componentHidden(java.awt.event.ComponentEvent e)  {
                AccessibleContainer.this.firePropertyChange(
                    AccessibleContext.ACCESSIBLE_STATE_PROPERTY,
                    AccessibleState.VISIBLE, null);
            }

            public void componentShown(java.awt.event.ComponentEvent e)  {
                AccessibleContainer.this.firePropertyChange(
                    AccessibleContext.ACCESSIBLE_STATE_PROPERTY,
                    null, AccessibleState.VISIBLE);
            }

            public void componentMoved(java.awt.event.ComponentEvent e)  {
            }

            public void componentResized(java.awt.event.ComponentEvent e)  {
            }
        } // inner class AccessibleContainerHandler

        protected java.awt.event.FocusListener accessibleFocusHandler = null;

        /**
        * Fire PropertyChange listener, if one is registered,
        * when focus events happen
        */
        protected class AccessibleFocusHandler implements java.awt.event.FocusListener {
            public void focusGained(java.awt.event.FocusEvent event) {
                AccessibleContainer.this.firePropertyChange(
                    AccessibleContext.ACCESSIBLE_STATE_PROPERTY,
                    null, AccessibleState.FOCUSED);
                if (Build.DEBUG) {
                    System.err.println("[" + getAccessibleRole() + "] " + getAccessibleName() + " is now focused");
                }
            }
            public void focusLost(java.awt.event.FocusEvent event) {
                AccessibleContainer.this.firePropertyChange(
                    AccessibleContext.ACCESSIBLE_STATE_PROPERTY,
                    AccessibleState.FOCUSED, null);
                if (Build.DEBUG) {
                    System.err.println("[" + getAccessibleRole() + "] " + getAccessibleName() + " is no longer focused");
                }
            }
        } // inner class AccessibleFocusHandler

        protected java.awt.event.ContainerListener accessibleContainerHandler = null;

        /**
        * Fire PropertyChange listener, if one is registered,
        * when children added/removed.
        */

        protected class AccessibleContainerHandler implements java.awt.event.ContainerListener {
            public void componentAdded(java.awt.event.ContainerEvent e) {
                java.awt.Component c = e.getChild();
                if (c != null && c instanceof javax.accessibility.Accessible) {
                    AccessibleContainer.this.firePropertyChange(
                        AccessibleContext.ACCESSIBLE_CHILD_PROPERTY,
                        null, ((javax.accessibility.Accessible) c).getAccessibleContext());
                }
            }
            public void componentRemoved(java.awt.event.ContainerEvent e) {
                java.awt.Component c = e.getChild();
                if (c != null && c instanceof javax.accessibility.Accessible) {
                    AccessibleContainer.this.firePropertyChange(
                        AccessibleContext.ACCESSIBLE_CHILD_PROPERTY,
                        ((javax.accessibility.Accessible) c).getAccessibleContext(), null);
                }
            }
        }

        protected int propertyChangeListenerCount = 0;

        /**
        * Add a PropertyChangeListener to the listener list.
        *
        * @param listener  The PropertyChangeListener to be added
        */
        public void addPropertyChangeListener(java.beans.PropertyChangeListener listener) {
            if (propertyChangeListenerCount++ == 0) {
                accessibleFocusHandler = new AccessibleFocusHandler();
                Container.this.addFocusListener(accessibleFocusHandler);

                accessibleContainerHandler = new AccessibleContainerHandler();
                Container.this.addContainerListener(accessibleContainerHandler);

                accessibleComponentHandler = new AccessibleComponentHandler();
                Container.this.addComponentListener(accessibleComponentHandler);
            }
            super.addPropertyChangeListener(listener);
        }

        /**
        * Remove a PropertyChangeListener from the listener list.
        * This removes a PropertyChangeListener that was registered
        * for all properties.
        *
        * @param listener  The PropertyChangeListener to be removed
        */
        public void removePropertyChangeListener(java.beans.PropertyChangeListener listener) {
            if (--propertyChangeListenerCount == 0) {
                Container.this.removeComponentListener(accessibleComponentHandler);
                accessibleComponentHandler = null;

                Container.this.removeContainerListener(accessibleContainerHandler);
                accessibleContainerHandler = null;

                Container.this.removeFocusListener(accessibleFocusHandler);
                accessibleFocusHandler = null;
            }
            super.removePropertyChangeListener(listener);
        }

        /** Gets the role of this object */
        public javax.accessibility.AccessibleRole getAccessibleRole() {
            return accessibleRole;
        }

        /** Gets the AccessibleText associated with this object presenting text on the display */
        public javax.accessibility.AccessibleText getAccessibleText() {

            if (disposed)
                return null;

            return accessibleText;
        }

        /**
        * Gets the current state set of this object.
        *
        * @return an instance of <code>AccessibleStateSet</code>
        *    containing the current state set of the object
        * @see AccessibleState
        */
        public javax.accessibility.AccessibleStateSet getAccessibleStateSet() {
            if (disposed)
                return AccessibleStateAdapter.getDefunctStateSet();

            try {
                return AccessibleStateAdapter.getAccessibleStateSet(Container.this,
                    unoAccessibleContext.getAccessibleStateSet());
            } catch (com.sun.star.uno.RuntimeException e) {
                return AccessibleStateAdapter.getDefunctStateSet();
            }
        }

        /** Returns the AccessibleSelection interface for this object */
        public javax.accessibility.AccessibleSelection getAccessibleSelection() {
            try {
                XAccessibleSelection unoAccessibleSelection = (XAccessibleSelection)
                    UnoRuntime.queryInterface(XAccessibleSelection.class, unoAccessibleContext);
                if (unoAccessibleSelection != null) {
                    return new AccessibleSelectionImpl(unoAccessibleSelection);
                }
            } catch (com.sun.star.uno.RuntimeException e) {
            }

            return null;
        }

        /** Gets the locale of the component */
        public java.util.Locale getLocale() throws java.awt.IllegalComponentStateException {
            try {
                com.sun.star.lang.Locale unoLocale = unoAccessible.getAccessibleContext().getLocale();
                return new java.util.Locale(unoLocale.Language, unoLocale.Country);
            } catch (IllegalAccessibleComponentStateException e) {
                throw new java.awt.IllegalComponentStateException(e.getMessage());
            } catch (com.sun.star.uno.RuntimeException e) {
                return super.getLocale();
            }
        }

        /*
        * AccessibleComponent
        */

        /** Returns the background color of the object */
        public java.awt.Color getBackground() {
            try {
                return new java.awt.Color(unoAccessibleComponent.getBackground());
            } catch (com.sun.star.uno.RuntimeException e) {
                return null;
            }
        }

        public void setBackground(java.awt.Color c) {
            // Not supported by UNO accessibility API
        }

        /** Returns the foreground color of the object */
        public java.awt.Color getForeground() {
            try {
                return new java.awt.Color(unoAccessibleComponent.getForeground());
            } catch (com.sun.star.uno.RuntimeException e) {
                return null;
            }
        }

        public void setForeground(java.awt.Color c) {
            // Not supported by UNO accessibility API
        }

        public java.awt.Cursor getCursor() {
            // Not supported by UNO accessibility API
            return null;
        }

        public void setCursor(java.awt.Cursor cursor) {
            // Not supported by UNO accessibility API
        }

        public java.awt.Font getFont() {
            // FIXME
            return null;
        }

        public void setFont(java.awt.Font f) {
            // Not supported by UNO accessibility API
        }

        public java.awt.FontMetrics getFontMetrics(java.awt.Font f) {
            // FIXME
            return null;
        }

        public boolean isEnabled() {
            return Container.this.isEnabled();
        }

        public void setEnabled(boolean b) {
            // Not supported by UNO accessibility API
        }

        public boolean isVisible() {
            return Container.this.isVisible();
        }

        public void setVisible(boolean b) {
            // Not supported by UNO accessibility API
        }

        public boolean isShowing() {
            return Container.this.isShowing();
        }

        public boolean contains(java.awt.Point p) {
            try {
                return unoAccessibleComponent.containsPoint(new com.sun.star.awt.Point(p.x, p.y));
            } catch (com.sun.star.uno.RuntimeException e) {
                return false;
            }
        }

        /** Returns the location of the object on the screen. */
        public java.awt.Point getLocationOnScreen() {
            try {
                com.sun.star.awt.Point unoPoint = unoAccessibleComponent.getLocationOnScreen();
                return new java.awt.Point(unoPoint.X, unoPoint.Y);
            } catch (com.sun.star.uno.RuntimeException e) {
                return null;
            }
        }

        /** Gets the location of this component in the form of a point specifying the component's top-left corner */
        public java.awt.Point getLocation() {
            try {
                com.sun.star.awt.Point unoPoint = unoAccessibleComponent.getLocation();
                return new java.awt.Point( unoPoint.X, unoPoint.Y );
            } catch (com.sun.star.uno.RuntimeException e) {
                return null;
            }
        }

        /** Moves this component to a new location */
        public void setLocation(java.awt.Point p) {
            // Not supported by UNO accessibility API
        }

        /** Gets the bounds of this component in the form of a Rectangle object */
        public java.awt.Rectangle getBounds() {
            try {
                com.sun.star.awt.Rectangle unoRect = unoAccessibleComponent.getBounds();
                return new java.awt.Rectangle(unoRect.X, unoRect.Y, unoRect.Width, unoRect.Height);
            } catch (com.sun.star.uno.RuntimeException e) {
                return null;
            }
        }

        /** Moves and resizes this component to conform to the new bounding rectangle r */
        public void setBounds(java.awt.Rectangle r) {
            // Not supported by UNO accessibility API
        }

        /** Returns the size of this component in the form of a Dimension object */
        public java.awt.Dimension getSize() {
            try {
                com.sun.star.awt.Size unoSize = unoAccessibleComponent.getSize();
                return new java.awt.Dimension(unoSize.Width, unoSize.Height);
            } catch (com.sun.star.uno.RuntimeException e) {
                return null;
            }
        }

        /** Resizes this component so that it has width d.width and height d.height */
        public void setSize(java.awt.Dimension d) {
            // Not supported by UNO accessibility API
        }

        /** Returns the Accessible child, if one exists, contained at the local coordinate Point */
        public javax.accessibility.Accessible getAccessibleAt(java.awt.Point p) {
            try {
                java.awt.Component c = AccessibleObjectFactory.getAccessibleComponent(
                    unoAccessibleComponent.getAccessibleAtPoint(new com.sun.star.awt.Point(p.x, p.y)));

                return (javax.accessibility.Accessible) c;
            } catch (com.sun.star.uno.RuntimeException e) {
                return null;
            }
        }

        public boolean isFocusTraversable() {
            return Container.this.isFocusable();
        }

        public void requestFocus() {
            unoAccessibleComponent.grabFocus();
        }
    }

    public String toString() {
        return UnoRuntime.generateOid(unoAccessible);
    }
}

