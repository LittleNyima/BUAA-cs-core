package com.oocourse.spec3.main;

import java.math.BigInteger;

import com.oocourse.spec3.exceptions.EqualGroupIdException;
import com.oocourse.spec3.exceptions.EqualPersonIdException;
import com.oocourse.spec3.exceptions.EqualRelationException;
import com.oocourse.spec3.exceptions.PersonIdNotFoundException;
import com.oocourse.spec3.exceptions.RelationNotFoundException;
import com.oocourse.spec3.exceptions.GroupIdNotFoundException;

public interface Network {
    
    /*@ public instance model non_null Person[] people;
      @ public instance model non_null Group[] groups;
      @ public instance model non_null int[] money;
      @*/
      
    //@ ensures \result == (\exists int i; 0 <= i && i < people.length; people[i].getId() == id);
    public /*@pure@*/ boolean contains(int id);
    
    /*@ public normal_behavior
      @ requires contains(id);
      @ ensures (\exists int i; 0 <= i && i < people.length; people[i].getId() == id && 
      @         \result == people[i]);
      @ also
      @ public normal_behavior
      @ requires !contains(id);
      @ ensures \result == null;
      @*/
    public /*@pure@*/ Person getPerson(int id);
    
    /*@ public normal_behavior
      @ requires !(\exists int i; 0 <= i && i < people.length; people[i].equals(person));
      @ assignable people, money;
      @ ensures people.length == \old(people.length) + 1;
      @ ensures money.length == people.length;
      @ ensures (\forall int i; 0 <= i && i < \old(people.length); 
      @         (\exists int j; 0 <= j && j < people.length; 
      @         people[j] == \old(people[i]) && money[j] == \old(money[i])));
      @ ensures (\exists int i; 0 <= i && i < people.length; people[i] == person && money[i] == 0);
      @ also
      @ public exceptional_behavior
      @ signals (EqualPersonIdException e) (\exists int i; 0 <= i && i < people.length; 
      @                                     people[i].equals(person));
      @*/
    public void addPerson(Person person) throws EqualPersonIdException;
    
    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2) && id1 != id2 && 
      @             !getPerson(id1).isLinked(getPerson(id2));
      @ assignable people;
      @ ensures people.length == \old(people.length);
      @ ensures (\forall int i; 0 <= i && i < \old(people.length); 
      @          (\exists int j; 0 <= j && j < people.length; people[j] == \old(people[i])));
      @ ensures (\forall int i; 0 <= i && i < people.length && \old(people[i].getId()) != id1 && 
      @     \old(people[i].getId()) != id2; \not_assigned(people[i]));
      @ ensures getPerson(id1).isLinked(getPerson(id2)) && getPerson(id2).isLinked(getPerson(id1));
      @ ensures getPerson(id1).queryValue(getPerson(id2)) == value;
      @ ensures getPerson(id2).queryValue(getPerson(id1)) == value;
      @ ensures (\forall int i; 0 <= i && i < \old(getPerson(id1).acquaintance.length); 
      @         \old(getPerson(id1).acquaintance[i]) == getPerson(id1).acquaintance[i] && 
      @          \old(getPerson(id1).value[i]) == getPerson(id1).value[i]);
      @ ensures (\forall int i; 0 <= i && i < \old(getPerson(id2).acquaintance.length); 
      @         \old(getPerson(id2).acquaintance[i]) == getPerson(id2).acquaintance[i] && 
      @          \old(getPerson(id2).value[i]) == getPerson(id2).value[i]);
      @ ensures getPerson(id1).value.length == getPerson(id1).acquaintance.length;
      @ ensures getPerson(id2).value.length == getPerson(id2).acquaintance.length;
      @ ensures \old(getPerson(id1).value.length) == getPerson(id1).acquaintance.length - 1;
      @ ensures \old(getPerson(id2).value.length) == getPerson(id2).acquaintance.length - 1;
      @ also
      @ public exceptional_behavior
      @ assignable \nothing;
      @ requires !contains(id1) || !contains(id2) || 
      @          (getPerson(id1).isLinked(getPerson(id2)) && id1 != id2);
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @ signals (EqualRelationException e) contains(id1) && contains(id2) && 
      @         getPerson(id1).isLinked(getPerson(id2));
      @*/
    public void addRelation(int id1, int id2, int value) throws 
        PersonIdNotFoundException, EqualRelationException;

    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2) && getPerson(id1).isLinked(getPerson(id2));
      @ ensures \result == getPerson(id1).queryValue(getPerson(id2));
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @ signals (RelationNotFoundException e) contains(id1) && contains(id2) && 
      @         !getPerson(id1).isLinked(getPerson(id2));
      @*/
    public /*@pure@*/ int queryValue(int id1, int id2) throws 
        PersonIdNotFoundException, RelationNotFoundException;

    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2);
      @ ensures \result.equals(getPerson(id1).getCharacter().xor(getPerson(id2).getCharacter()));
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @*/
    public /*@pure@*/ BigInteger queryConflict(int id1, int id2) throws PersonIdNotFoundException;
    
    /*@ public normal_behavior
      @ requires contains(id);
      @ ensures \result == getPerson(id).getAcquaintanceSum();
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id);
      @*/
    public /*@pure@*/ int queryAcquaintanceSum(int id) throws PersonIdNotFoundException;
    
    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2);
      @ ensures \result == getPerson(id1).getAge() - getPerson(id2).getAge();
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @*/
    public /*@pure@*/ int compareAge(int id1, int id2) throws PersonIdNotFoundException;
    
    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2);
      @ ensures \result == getPerson(id1).getName().compareTo(getPerson(id2).getName());
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @*/
    public /*@pure@*/ int compareName(int id1, int id2) throws PersonIdNotFoundException;

    //@ ensures \result == people.length;
    public /*@pure@*/ int queryPeopleSum();

    /*@ public normal_behavior
      @ requires contains(id);
      @ ensures \result == (\sum int i; 0 <= i && i < people.length && 
      @                     compareName(id, people[i].getId()) > 0; 1) + 1;
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id);
      @*/
    public /*@pure@*/ int queryNameRank(int id) throws PersonIdNotFoundException;
    
    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2);
      @ ensures \result == (\exists Person[] array; array.length >= 2; 
      @                     array[0].equals(getPerson(id1)) && 
      @                     array[array.length - 1].equals(getPerson(id2)) &&
      @                      (\forall int i; 0 <= i && i < array.length - 1; 
      @                      array[i].isLinked(array[i + 1]) == true));
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @*/
    public /*@pure@*/ boolean isCircle(int id1, int id2) throws PersonIdNotFoundException;

    /*@ public normal_behavior
      @ requires !(\exists int i; 0 <= i && i < groups.length; groups[i].equals(group));
      @ assignable groups;
      @ ensures groups.length == \old(groups.length) + 1;
      @ ensures (\forall int i; 0 <= i && i < \old(groups.length); 
      @          (\exists int j; 0 <= j && j < groups.length; groups[j] == (\old(groups[i]))));
      @ ensures (\exists int i; 0 <= i && i < groups.length; groups[i] == group);
      @ also
      @ public exceptional_behavior
      @ signals (EqualGroupIdException e) (\exists int i; 0 <= i && i < groups.length; 
      @                                     groups[i].equals(group));
      @*/
    public void addGroup(Group group) throws EqualGroupIdException;
    
    /*@ ensures (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id && 
      @ \result == groups[i]);
      @*/
    public /*@pure@*/ Group getGroup(int id);
    
    /*@ public normal_behavior
      @ requires (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id2) &&
      @           (\exists int i; 0 <= i && i < people.length; people[i].getId() == id1) &&
      @            getGroup(id2).hasPerson(getPerson(id1)) == false && 
      @             getGroup(id2).people.length < 1111;
      @ assignable groups;
      @ ensures (\forall int i; 0 <= i < groups.length; \not_assigned(groups[i]));
      @ ensures (\forall Person i; \old(getGroup(id2).hasPerson(i)); 
      @          getGroup(id2).hasPerson(i));
      @ ensures \old(getGroup(id2).people.length) == getGroup(id2).people.length - 1;
      @ ensures getGroup(id2).hasPerson(getPerson(id1));
      @ also
      @ public exceptional_behavior
      @ signals (GroupIdNotFoundException e) !(\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id2);
      @ signals (PersonIdNotFoundException e) (\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id2) && !(\exists int i; 0 <= i && i < people.length; 
      @           people[i].getId() == id1);
      @ signals (EqualPersonIdException e) (\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id2) && (\exists int i; 0 <= i && i < people.length; 
      @           people[i].getId() == id1) && getGroup(id2).hasPerson(getPerson(id1));
      @*/
    public void addtoGroup(int id1, int id2) throws GroupIdNotFoundException, 
        PersonIdNotFoundException, EqualPersonIdException;

    //@ ensures \result == groups.length;
    public /*@pure@*/ int queryGroupSum();

    /*@ public normal_behavior
      @ requires (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id);
      @ ensures \result == getGroup(id).people.length;
      @ also
      @ public exceptional_behavior
      @ signals (GroupIdNotFoundException e) !(\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id);
      @*/
    public /*@pure@*/ int queryGroupPeopleSum(int id) throws GroupIdNotFoundException;
    
    /*@ public normal_behavior
      @ requires (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id);
      @ ensures \result == getGroup(id).getRelationSum();
      @ also
      @ public exceptional_behavior
      @ signals (GroupIdNotFoundException e) !(\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id);
      @*/
    public /*@pure@*/ int queryGroupRelationSum(int id) throws GroupIdNotFoundException;

    /*@ public normal_behavior
      @ requires (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id);
      @ ensures \result == getGroup(id).getValueSum();
      @ also
      @ public exceptional_behavior
      @ signals (GroupIdNotFoundException e) !(\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id);
      @*/
    public /*@pure@*/ int queryGroupValueSum(int id) throws GroupIdNotFoundException;

    /*@ public normal_behavior
      @ requires (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id);
      @ ensures \result == getGroup(id).getConflictSum();
      @ also
      @ public exceptional_behavior
      @ signals (GroupIdNotFoundException e) !(\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id);
      @*/
    public /*@pure@*/ BigInteger queryGroupConflictSum(int id) throws GroupIdNotFoundException;

    /*@ public normal_behavior
      @ requires (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id);
      @ ensures \result == getGroup(id).getAgeMean();
      @ also
      @ public exceptional_behavior
      @ signals (GroupIdNotFoundException e) !(\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id);
      @*/
    public /*@pure@*/ int queryGroupAgeMean(int id) throws GroupIdNotFoundException;
    
    /*@ public normal_behavior
      @ requires (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id);
      @ ensures \result == getGroup(id).getAgeVar();
      @ also
      @ public exceptional_behavior
      @ signals (GroupIdNotFoundException e) !(\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id);
      @*/
    public /*@pure@*/ int queryGroupAgeVar(int id) throws GroupIdNotFoundException;

    /*@ ensures \result == (\sum int i; 0 <= i < people.length && people[i].getAge() >= l 
      @                    && people[i].getAge() <= r; 1);
      @*/
    public /*@pure@*/ int queryAgeSum(int l, int r);

    /*@ public normal_behavior
      @ requires (\exists int i; 0 <= i && i < groups.length; groups[i].getId() == id2) &&
      @           (\exists int i; 0 <= i && i < people.length; people[i].getId() == id1) &&
      @            getGroup(id2).hasPerson(getPerson(id1)) == true;
      @ assignable groups;
      @ ensures (\forall int i; 0 <= i < groups.length; \not_assigned(groups[i]));
      @ ensures (\forall Person i; getGroup(id2).hasPerson(i); 
      @          \old(getGroup(id2).hasPerson(i)));
      @ ensures \old(getGroup(id2).people.length) == getGroup(id2).people.length + 1;
      @ ensures getGroup(id2).hasPerson(getPerson(id1)) == false;
      @ also
      @ public exceptional_behavior
      @ signals (GroupIdNotFoundException e) !(\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id2);
      @ signals (PersonIdNotFoundException e) (\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id2) && !(\exists int i; 0 <= i && i < people.length; 
      @           people[i].getId() == id1);
      @ signals (EqualPersonIdException e) (\exists int i; 0 <= i && i < groups.length; 
      @          groups[i].getId() == id2) && (\exists int i; 0 <= i && i < people.length; 
      @           people[i].getId() == id1) && !getGroup(id2).hasPerson(getPerson(id1));
      @*/
    public void delFromGroup(int id1, int id2) 
            throws GroupIdNotFoundException, PersonIdNotFoundException, EqualPersonIdException;
    
    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2) && id1 == id2;
      @ ensures \result == 0;
      @ also
      @ public normal_behavior
      @ requires contains(id1) && contains(id2) && id1 != id2 && 
      @          !isCircle(id1, id2);
      @ ensures \result == -1;
      @ also
      @ public normal_behavior
      @ requires contains(id1) && contains(id2) && id1 != id2 && 
      @          isCircle(id1, id2);
      @ ensures (\exists Person[] pathM; 
      @         pathM.length >= 2 && 
      @         pathM[0].equals(getPerson(id1)) && pathM[pathM.length - 1].equals(getPerson(id2))&& 
      @         (\forall int i; 1 <= i && i < pathM.length; pathM[i - 1].isLinked(pathM[i]));
      @         (\forall Person[] path; 
      @         path.length >= 2 &&
      @         path[0].equals(getPerson(id1)) && path[path.length - 1].equals(getPerson(id2)) && 
      @         (\forall int i; 1 <= i && i < path.length; path[i - 1].isLinked(path[i])); 
      @         (\sum int i; 1 <= i && i < path.length; path[i - 1].queryValue(path[i])) >= 
      @         (\sum int i; 1 <= i && i < pathM.length; pathM[i - 1].queryValue(pathM[i]))) && 
      @         \result==(\sum int i; 1 <= i && i < pathM.length; pathM[i-1].queryValue(pathM[i])));
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @*/
    public /*@pure@*/ int queryMinPath(int id1, int id2) throws PersonIdNotFoundException;

    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2) && id1 == id2;
      @ ensures \result == true;
      @ also
      @ public normal_behavior
      @ requires contains(id1) && contains(id2) && id1 != id2;
      @ ensures \result == (\exists Person[] pathM, path; pathM.length >= 3 && path.length >= 2 &&
      @         pathM[0].equals(getPerson(id1)) && pathM[pathM.length - 1].equals(getPerson(id2))&& 
      @         (\forall int i; 1 <= i && i < pathM.length; pathM[i - 1].isLinked(pathM[i])) &&
      @         path[0].equals(getPerson(id1)) && path[path.length - 1].equals(getPerson(id2)) && 
      @         (\forall int i; 1 <= i && i < path.length; path[i - 1].isLinked(path[i]));
      @         (\sum int i,j; 0 <= i && i < pathM.length && 0 <= j && j < path.length &&
      @         pathM[i].equals(path[j]); 1) == 2);
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @*/
    public /*@pure@*/ boolean queryStrongLinked(int id1, int id2) throws PersonIdNotFoundException;
    
    /*@ ensures \result == 
      @         (\sum int i; 0 <= i && i < people.length && 
      @         (\forall int j; 0 <= j && j < i; !isCircle(people[i].getId(), people[j].getId()));
      @         1);
      @*/
    public /*@pure@*/ int queryBlockSum();

    /*@ public normal_behavior
      @ requires contains(id1) && contains(id2) && id1 != id2;
      @ assignable money;
      @ ensures (\exists int i; 0 <= i && i < people.length && people[i].getId() == id1;
      @         money[i] == \old(money[i] - value));
      @ ensures (\exists int i; 0 <= i && i < people.length && people[i].getId() == id2;
      @         money[i] == \old(money[i] + value));
      @ ensures (\forall int i; 0 <= i && i < people.length && 
      @         people[i].getId() != id1 && people[i].getId() != id2;
      @         money[i] == \old(money[i]));
      @ also 
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id1) || !contains(id2);
      @ signals (EqualPersonIdException e) contains(id1) && id1 == id2;
      @*/
    public void borrowFrom(int id1, int id2, int value) 
            throws PersonIdNotFoundException, EqualPersonIdException;
    
    /*@ public normal_behavior
      @ requires contains(id);
      @ ensures (\exists int i; 0 <= i && i < people.length && people[i].getId() == id;
      @          money[i] == \result);
      @ also
      @ public exceptional_behavior
      @ signals (PersonIdNotFoundException e) !contains(id);
      @*/
    public /*@pure@*/ int queryMoney(int id) throws  PersonIdNotFoundException;
}
