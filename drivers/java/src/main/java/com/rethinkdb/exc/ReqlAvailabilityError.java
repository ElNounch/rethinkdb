// Autogenerated by metajava.py.
// Do not edit this file directly.
// The template for this file is located at:
// ../../../../../../../templates/Exception.java
package com.rethinkdb;

public class ReqlAvailabilityError extends ReqlRuntimeError {
    public ReqlAvailabilityError() {}

    public ReqlAvailabilityError(String message) {
        super(message);
    }

    public ReqlAvailabilityError(String format, Object... args) {
        super(String.format(format, args));
    }

    public ReqlAvailabilityError(String message, Throwable cause) {
        super(message, cause);
    }

    public ReqlAvailabilityError(Throwable cause) {
        super(cause);
    }
}