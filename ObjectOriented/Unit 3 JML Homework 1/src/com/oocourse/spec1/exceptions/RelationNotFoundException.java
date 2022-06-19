package com.oocourse.spec1.exceptions;

@SuppressWarnings("serial")
public class RelationNotFoundException extends Exception {
    private String message;

    public RelationNotFoundException() {
        super("");
        message = String.format("can't find the Relation.");
    }

    public void print() {
        System.out.println(message);
    }
}
