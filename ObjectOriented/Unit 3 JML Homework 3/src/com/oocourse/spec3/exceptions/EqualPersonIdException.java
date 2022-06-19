package com.oocourse.spec3.exceptions;

@SuppressWarnings("serial")
public class EqualPersonIdException extends Exception {
    private String message;

    public EqualPersonIdException() {
        super("");
        message = String.format("epi");
    }

    public void print() {
        System.out.println(message);
    }
}
