package com.oocourse.spec2.exceptions;

@SuppressWarnings("serial")
public class EqualGroupIdException extends Exception {
    private String message;

    public EqualGroupIdException() {
        super("");
        message = String.format("egi");
    }

    public void print() {
        System.out.println(message);
    }
}
