package com.oocourse.spec1.exceptions;

@SuppressWarnings("serial")
public class EqualRelationException extends Exception {
    private String message;

    public EqualRelationException() {
        super("");
        message = String.format("there is a same Relation.");
    }

    public void print() {
        System.out.println(message);
    }
}
