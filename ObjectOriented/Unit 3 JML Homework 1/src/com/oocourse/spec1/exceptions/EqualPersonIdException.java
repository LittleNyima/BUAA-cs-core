package com.oocourse.spec1.exceptions;

@SuppressWarnings("serial")
public class EqualPersonIdException extends Exception {
    private String message;

    public EqualPersonIdException() {
        super("");
        message = String.format("there is another Person has the same id.");
    }

    public void print() {
        System.out.println(message);
    }
}
