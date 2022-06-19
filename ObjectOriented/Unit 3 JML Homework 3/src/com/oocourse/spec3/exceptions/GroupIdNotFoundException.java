package com.oocourse.spec3.exceptions;

@SuppressWarnings("serial")
public class GroupIdNotFoundException extends Exception {
    private String message;

    public GroupIdNotFoundException() {
        super("");
        message = String.format("ginf");
    }

    public void print() {
        System.out.println(message);
    }
}
