package com.oocourse.spec1.main;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.math.BigInteger;
import java.util.Scanner;

import com.oocourse.spec1.exceptions.EqualPersonIdException;
import com.oocourse.spec1.exceptions.EqualRelationException;
import com.oocourse.spec1.exceptions.PersonIdNotFoundException;
import com.oocourse.spec1.exceptions.RelationNotFoundException;

public class Runner {

    private String[] cmds;
    private Network network;
    private Class<? extends Person> personClass;
    private Class<? extends Network> networkClass;
    private Constructor<? extends Person> personConstructor;
    private Constructor<? extends Network> networkConstructor;

    public Runner(Class<? extends Person> personClass, Class<? extends Network> networkClass)
            throws NoSuchMethodException, SecurityException {
        this.personClass = personClass;
        this.networkClass = networkClass;
        personConstructor = this.personClass.getConstructor(
                int.class, String.class, BigInteger.class, int.class);
        networkConstructor = this.networkClass.getConstructor();
    }

    public void run()
            throws InstantiationException, IllegalAccessException, 
            IllegalArgumentException, InvocationTargetException {
        Scanner cin = new Scanner(System.in);
        this.network = (Network) this.networkConstructor.newInstance();
        while (cin.hasNextLine()) {
            String cmd = cin.nextLine();
            cmds = cmd.split(" ");
            if (cmds[0].equals("add_person")) {
                addPerson();
            } else if (cmds[0].equals("add_relation")) {
                addRelation();
            } else if (cmds[0].equals("query_value")) {
                queryValue();
            } else if (cmds[0].equals("query_conflict")) {
                queryConflict();
            } else if (cmds[0].equals("query_acquaintance_sum")) {
                queryAcquaintanceSum();
            } else if (cmds[0].equals("compare_age")) {
                compareAge();
            } else if (cmds[0].equals("compare_name")) {
                compareName();
            } else if (cmds[0].equals("query_people_sum")) {
                queryPeopleSum();
            } else if (cmds[0].equals("query_name_rank")) {
                queryNameRank();
            } else if (cmds[0].equals("query_circle")) {
                queryCircle();
            } else {
                assert (false);
            }
        }
        cin.close();
    }

    private void queryCircle() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        boolean ret = false;
        try {
            ret = network.isCircle(id1, id2);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        if (ret == true) {
            System.out.println("they will in circle");
        } else {
            System.out.println("they won't in circle");
        }
    }

    private void queryNameRank() {
        int id = Integer.parseInt(cmds[1]);
        int ret = 0;
        try {
            ret = network.queryNameRank(id);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(String.format("Ok, the rank of %s is %s.", id, ret));
    }

    private void queryPeopleSum() {
        int ret = network.queryPeopleSum();
        System.out.println(String.format("Ok, the sum of people is %s.", ret));
    }

    private void compareName() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        int ret = 0;
        try {
            ret = network.compareName(id1, id2);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        String r;
        if (ret < 0) {
            r = "<";
        } else if (ret == 0) {
            r = "=";
        } else {
            r = ">";
        }
        System.out.println(String.format("Ok, the Age of %s %s the Age of %s.", id1, r, id2));
    }

    private void compareAge() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        int ret = 0;
        try {
            ret = network.compareAge(id1, id2);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        String r;
        if (ret < 0) {
            r = "<";
        } else if (ret == 0) {
            r = "=";
        } else {
            r = ">";
        }
        System.out.println(String.format("Ok, the Age of %s %s the Age of %s.", id1, r, id2));
    }

    private void queryAcquaintanceSum() {
        int id = Integer.parseInt(cmds[1]);
        int ret = 0;
        try {
            ret = network.queryAcquaintanceSum(id);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(String.format("Ok, Acquaintance sum of %s is: %s.", id, ret));
    }

    private void queryConflict() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        BigInteger ret = BigInteger.ZERO;
        try {
            ret = network.queryConflict(id1, id2);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(String.format("Ok, conflict is: %s.", ret));
    }

    private void queryValue() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        int ret = 0;
        try {
            ret = network.queryValue(id1, id2);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        } catch (RelationNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(String.format("Ok, value is: %s.", ret));
    }

    private void addRelation() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        int value = Integer.parseInt(cmds[3]);
        try {
            network.addRelation(id1, id2, value);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        } catch (EqualRelationException e) {
            e.print();
            return;
        }
        System.out.println(String.format("Ok, Relation added."));
    }

    private void addPerson()
            throws InstantiationException, IllegalAccessException, 
            IllegalArgumentException, InvocationTargetException {
        int id = Integer.parseInt(cmds[1]);
        String name = cmds[2];
        BigInteger character = new BigInteger(cmds[3]);
        int age = Integer.parseInt(cmds[4]);
        try {
            network.addPerson((Person) this.personConstructor.newInstance(
                    id, name, character, age));
        } catch (EqualPersonIdException e) {
            e.print();
            return;
        }
        System.out.println(String.format("Ok, Person added."));
    }

}
