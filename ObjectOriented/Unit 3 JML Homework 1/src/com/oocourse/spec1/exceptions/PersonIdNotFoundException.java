package com.oocourse.spec1.exceptions;

@SuppressWarnings("serial")
public class PersonIdNotFoundException extends Exception {
    private String message;

    public PersonIdNotFoundException() {
        super("");
        message = String.format("Can't find a Person has this id.");
    }

    public void print() {
        System.out.println(message);
    }
}
