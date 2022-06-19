package com.oocourse.spec3.exceptions;

@SuppressWarnings("serial")
public class RelationNotFoundException extends Exception {
    private String message;

    public RelationNotFoundException() {
        super("");
        message = String.format("rnf");
    }

    public void print() {
        System.out.println(message);
    }
}
