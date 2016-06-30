#
#    ICRAR - International Centre for Radio Astronomy Research
#    (c) UWA - The University of Western Australia, 2016
#    Copyright by UWA (in the framework of the ICRAR)
#    All rights reserved
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#    MA 02111-1307  USA
#
"""
Module containing common exceptions used throughout Daliuge.
In particular DaliugeException should be the root of all our exceptions.
"""

class DaliugeException(Exception):
    """
    The parent of all exceptions thrown by Daliuge
    """

class InvalidGraphException(DaliugeException):
    """
    An exception thrown when an invalid graph, or part of a graph, is given to
    Daliuge.
    """

class NoSessionException(DaliugeException):
    """
    An exception thrown when a session ID is pointing to a non-existing session
    """

    def __init__(self, session_id):
        self._session_id = session_id

    def __repr__(self):
        return "NoSessionException <session_id: %s>" % (self._session_id)

class SessionAlreadyExistsException(DaliugeException):
    """
    An exception thrown when a session ID is pointing to an existing session
    but is meant to be used as the ID of a new session.
    """

    def __init__(self, session_id):
        self._session_id = session_id

    def __repr__(self):
        return "SessionAlreadyExistsException <session_id: %s>" % (self._session_id)

class InvalidSessionState(DaliugeException):
    """
    An exception thrown when an operation is requested on a session that is not
    in the expected state for that operation.
    """

class SubManagerException(DaliugeException):
    """
    An exception thrown by composite drop managers when there was an error
    invoking one of their underlying drop managers.
    """