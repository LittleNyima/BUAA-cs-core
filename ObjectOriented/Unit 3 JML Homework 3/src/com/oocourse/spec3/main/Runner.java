package com.oocourse.spec3.main;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.math.BigInteger;
import java.util.Scanner;

import com.oocourse.spec3.exceptions.EqualGroupIdException;
import com.oocourse.spec3.exceptions.EqualPersonIdException;
import com.oocourse.spec3.exceptions.EqualRelationException;
import com.oocourse.spec3.exceptions.GroupIdNotFoundException;
import com.oocourse.spec3.exceptions.PersonIdNotFoundException;
import com.oocourse.spec3.exceptions.RelationNotFoundException;

public class Runner {

    private String[] cmds;
    private Network network;
    private Class<? extends Person> personClass;
    private Class<? extends Network> networkClass;
    private Class<? extends Group> groupClass;
    private Constructor<? extends Person> personConstructor;
    private Constructor<? extends Network> networkConstructor;
    private Constructor<? extends Group> groupConstructor;
    private Scanner cin;

    public Runner(Class<? extends Person> personClass, Class<? extends Network> networkClass,
            Class<? extends Group> groupClass) throws NoSuchMethodException, SecurityException {
        this.personClass = personClass;
        this.networkClass = networkClass;
        this.groupClass = groupClass;
        personConstructor = this.personClass.getConstructor(
                int.class, String.class, BigInteger.class, int.class);
        networkConstructor = this.networkClass.getConstructor();
        groupConstructor = this.groupClass.getConstructor(int.class);
        cin = new Scanner(System.in);
    }

    public void run()
            throws InstantiationException, IllegalAccessException, 
            IllegalArgumentException, InvocationTargetException {
        this.network = (Network) this.networkConstructor.newInstance();
        while (cin.hasNextLine()) {
            String cmd = cin.nextLine();
            cmds = cmd.split(" ");
            if (cmds[0].equals("ap")) {
                addPerson();
            } else if (cmds[0].equals("ar")) {
                addRelation();
            } else if (cmds[0].equals("qv")) {
                queryValue();
            } else if (cmds[0].equals("qc")) {
                queryConflict();
            } else if (cmds[0].equals("qas")) {
                queryAcquaintanceSum();
            } else if (cmds[0].equals("ca")) {
                compareAge();
            } else if (cmds[0].equals("cn")) {
                compareName();
            } else if (cmds[0].equals("qps")) {
                queryPeopleSum();
            } else if (cmds[0].equals("qnr")) {
                queryNameRank();
            } else if (cmds[0].equals("qci")) {
                queryCircle();
            } else if (cmds[0].equals("ag")) {
                addGroup();
            } else if (cmds[0].equals("atg")) {
                addtoGroup();
            } else if (cmds[0].equals("qgs")) {
                queryGroupSum();
            } else if (cmds[0].equals("qgps")) {
                queryGroupPeopleSum();
            } else if (cmds[0].equals("qgrs")) {
                queryGroupRelationSum();
            } else if (cmds[0].equals("qgvs")) {
                queryGroupValueSum();
            } else if (cmds[0].equals("qgcs")) {
                queryGroupConflictSum();
            } else if (cmds[0].equals("qgam")) {
                queryGroupAgeMean();
            } else if (cmds[0].equals("qgav")) {
                queryGroupAgeVar();
            } else if (cmds[0].equals("qasu")) {
                queryAgeSum();
            } else if (cmds[0].equals("dfg")) {
                delFromGroup();
            } else if (cmds[0].equals("qmp")) {
                queryMinPath();
            } else if (cmds[0].equals("qsl")) {
                queryStrongLinked();
            } else if (cmds[0].equals("qbs")) {
                queryBlockSum();
            } else if (cmds[0].equals("bf")) {
                borrowFrom();
            } else if (cmds[0].equals("qm")) {
                query_money(); } 
        }
        cin.close();
    }

    private void query_money() {
        int id = Integer.parseInt(cmds[1]);
        int ret = 0;
        try {
            ret = network.queryMoney(id);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void borrowFrom() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        int value = Integer.parseInt(cmds[3]);
        try {
            network.borrowFrom(id1, id2, value);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        } catch (EqualPersonIdException e) {
            e.print();
        }
        System.out.println("Ok");
    }

    private void queryBlockSum() {
        System.out.println(network.queryBlockSum());
    }

    private void queryStrongLinked() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        boolean ret = false;
        try {
            ret = network.queryStrongLinked(id1, id2);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void queryMinPath() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        int ret = 0;
        try {
            ret = network.queryMinPath(id1, id2);
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void delFromGroup() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        try {
            network.delFromGroup(id1, id2);
        } catch (GroupIdNotFoundException e) {
            e.print();
            return;
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        } catch (EqualPersonIdException e) {
            e.print();
            return;
        }
        System.out.println("Ok");
    }

    private void queryAgeSum() {
        int l = Integer.parseInt(cmds[1]);
        int r = Integer.parseInt(cmds[2]);
        System.out.println(network.queryAgeSum(l, r));
    }

    private void queryGroupAgeVar() {
        int id = Integer.parseInt(cmds[1]);
        int ret = 0;
        try {
            ret = network.queryGroupAgeVar(id);
        } catch (GroupIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void queryGroupAgeMean() {
        int id = Integer.parseInt(cmds[1]);
        int ret = 0;
        try {
            ret = network.queryGroupAgeMean(id);
        } catch (GroupIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void queryGroupConflictSum() {
        int id = Integer.parseInt(cmds[1]);
        BigInteger ret = BigInteger.ZERO;
        try {
            ret = network.queryGroupConflictSum(id);
        } catch (GroupIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void queryGroupValueSum() {
        int id = Integer.parseInt(cmds[1]);
        int ret = 0;
        try {
            ret = network.queryGroupValueSum(id);
        } catch (GroupIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void queryGroupRelationSum() {
        int id = Integer.parseInt(cmds[1]);
        int ret = 0;
        try {
            ret = network.queryGroupRelationSum(id);
        } catch (GroupIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void queryGroupPeopleSum() {
        int id = Integer.parseInt(cmds[1]);
        int ret = 0;
        try {
            ret = network.queryGroupPeopleSum(id);
        } catch (GroupIdNotFoundException e) {
            e.print();
            return;
        }
        System.out.println(ret);
    }

    private void queryGroupSum() {
        int ret = network.queryGroupSum();
        System.out.println(ret);
    }

    private void addtoGroup() {
        int id1 = Integer.parseInt(cmds[1]);
        int id2 = Integer.parseInt(cmds[2]);
        try {
            network.addtoGroup(id1, id2);
        } catch (GroupIdNotFoundException e) {
            e.print();
            return;
        } catch (PersonIdNotFoundException e) {
            e.print();
            return;
        } catch (EqualPersonIdException e) {
            e.print();
            return;
        }
        System.out.println("Ok");
    }

    private void addGroup()
            throws InstantiationException, IllegalAccessException, 
            IllegalArgumentException, InvocationTargetException {
        int id = Integer.parseInt(cmds[1]);
        try {
            network.addGroup((Group) this.groupConstructor.newInstance(id));
        } catch (EqualGroupIdException e) {
            e.print();
            return;
        }
        System.out.println("Ok");
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
            System.out.println("1");
        } else {
            System.out.println("0");
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
        System.out.println(ret);
    }

    private void queryPeopleSum() {
        int ret = network.queryPeopleSum();
        System.out.println(ret);
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
        System.out.println(r);
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
        System.out.println(r);
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
        System.out.println(ret);
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
        System.out.println(ret);
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
        System.out.println(ret);
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
        System.out.println(String.format("Ok"));
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
        System.out.println(String.format("Ok"));
    }
}
