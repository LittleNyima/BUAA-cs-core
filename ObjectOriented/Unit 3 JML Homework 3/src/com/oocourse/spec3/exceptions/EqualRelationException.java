package com.oocourse.spec3.exceptions;

@SuppressWarnings("serial")
public class EqualRelationException extends Exception {
    private String message;

    public EqualRelationException() {
        super("");
        message = String.format("er");
    }

    public void print() {
        System.out.println(message);
    }
}
