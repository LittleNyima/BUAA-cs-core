package com.oocourse.spec2.exceptions;

@SuppressWarnings("serial")
public class PersonIdNotFoundException extends Exception {
    private String message;

    public PersonIdNotFoundException() {
        super("");
        message = String.format("pinf");
    }

    public void print() {
        System.out.println(message);
    }
}
