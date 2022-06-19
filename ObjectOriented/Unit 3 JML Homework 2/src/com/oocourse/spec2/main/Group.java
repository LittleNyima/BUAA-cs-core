package com.oocourse.spec2.main;

import java.math.BigInteger;

public interface Group {
    
    
    /*@ public instance model non_null int id;
      @ public instance model non_null Person[] people;
      @*/
      
    //@ ensures \result == id;
    public /*@pure@*/ int getId();
    
    /*@ also
      @ public normal_behavior
      @ requires obj != null && obj instanceof Group;
      @ assignable \nothing;
      @ ensures \result == (((Group) obj).getId() == id);
      @ also
      @ public normal_behavior
      @ requires obj == null || !(obj instanceof Group);
      @ assignable \nothing;
      @ ensures \result == false;
      @*/
    public /*@pure@*/ boolean equals(Object obj);
    
    public void addPerson(Person person);
    
    //@ ensures \result == (\exists int i; 0 <= i && i < people.length; people[i].equals(person));
    public /*@pure@*/ boolean hasPerson(Person person);
    
    /*@ ensures \result == (\sum int i; 0 <= i && i < people.length; 
      @          (\sum int j; 0 <= j && j < people.length && people[i].isLinked(people[j]); 1));
      @*/
    public /*@pure@*/ int getRelationSum();
    
    /*@ ensures \result == (\sum int i; 0 <= i && i < people.length; 
      @          (\sum int j; 0 <= j && j < people.length && 
      @           people[i].isLinked(people[j]); people[i].queryValue(people[j])));
      @*/
    public /*@pure@*/ int getValueSum();

    /*@ public normal_behavior
      @ requires people.length > 0;
      @ ensures (\exists BigInteger[] temp; 
      @          temp.length == people.length && temp[0] == people[0].getCharacter();
      @           (\forall int i; 1 <= i && i < temp.length; 
      @            temp[i] == temp[i-1].xor(people[i].getCharacter())) && 
      @             \result == temp[temp.length - 1]);
      @ also 
      @ public normal_behavior
      @ requires people.length == 0;
      @ ensures \result == BigInteger.ZERO;
      @*/
    public /*@pure@*/ BigInteger getConflictSum();
    
    /*@ ensures \result == people.length == 0? 0 : 
      @          ((\sum int i; 0 <= i && i < people.length; people[i].getAge()) / people.length);
      @*/
    public /*@pure@*/ int getAgeMean();
    
    /*@ ensures \result == people.length == 0? 0 : ((\sum int i; 0 <= i && i < people.length; 
      @          (people[i].getAge() - getAgeMean()) * (people[i].getAge() - getAgeMean())) / 
      @           people.length);
      @*/
    public /*@pure@*/ int getAgeVar();
}