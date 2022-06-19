import com.oocourse.spec1.exceptions.EqualPersonIdException;
import com.oocourse.spec1.exceptions.EqualRelationException;
import com.oocourse.spec1.exceptions.PersonIdNotFoundException;
import com.oocourse.spec1.exceptions.RelationNotFoundException;
import com.oocourse.spec1.main.Network;
import com.oocourse.spec1.main.Person;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Queue;

public class MyNetwork implements Network {

    private ArrayList<Person> people;
    //private HashMap<Person, Boolean> hm;

    public MyNetwork() {
        this.people = new ArrayList<>();
    }

    @Override
    public boolean contains(int id) {
        for (Person person : people) {
            if (person.getId() == id) {
                return true;
            }
        }
        return false;
    }

    @Override
    public Person getPerson(int id) {
        for (Person person : people) {
            if (person.getId() == id) {
                return person;
            }
        }
        return null;
    }

    @Override
    public void addPerson(Person person) throws EqualPersonIdException {
        for (Person pers : people) {
            if (person.getId() == pers.getId()) {
                throw new EqualPersonIdException();
            }
        }
        people.add(person);
    }

    @Override
    public void addRelation(int id1, int id2, int value) throws
            PersonIdNotFoundException, EqualRelationException {
        Person person1 = null;
        Person person2 = null;
        for (Person person : people) {
            if (person.getId() == id1) {
                person1 = person;
            }
            if (person.getId() == id2) {
                person2 = person;
            }
        }
        if (person1 == null || person2 == null) {
            throw new PersonIdNotFoundException();
        }
        if (id1 == id2) {
            return;
        }
        if (person1.isLinked(person2)) {
            throw new EqualRelationException();
        }
        ((MyPerson) person1).addRelation(person2, value);
        ((MyPerson) person2).addRelation(person1, value);
    }

    @Override
    public int queryValue(int id1, int id2) throws
            PersonIdNotFoundException, RelationNotFoundException {
        Person person1 = null;
        Person person2 = null;
        for (Person person : people) {
            if (person.getId() == id1) {
                person1 = person;
            }
            if (person.getId() == id2) {
                person2 = person;
            }
        }
        if (person1 == null || person2 == null) {
            throw new PersonIdNotFoundException();
        }
        if (!person1.isLinked(person2)) {
            throw new RelationNotFoundException();
        }
        return person1.queryValue(person2);
    }

    @Override
    public BigInteger queryConflict(int id1, int id2) throws PersonIdNotFoundException {
        Person person1 = null;
        Person person2 = null;
        for (Person person : people) {
            if (person.getId() == id1) {
                person1 = person;
            }
            if (person.getId() == id2) {
                person2 = person;
            }
        }
        if (person1 == null || person2 == null) {
            throw new PersonIdNotFoundException();
        }
        return person1.getCharacter().xor(person2.getCharacter());
    }

    @Override
    public int queryAcquaintanceSum(int id) throws PersonIdNotFoundException {
        Person person = null;
        for (Person pers : people) {
            if (pers.getId() == id) {
                person = pers;
            }
        }
        if (person == null) {
            throw new PersonIdNotFoundException();
        }
        return person.getAcquaintanceSum();
    }

    @Override
    public int compareAge(int id1, int id2) throws PersonIdNotFoundException {
        Person person1 = null;
        Person person2 = null;
        for (Person person : people) {
            if (person.getId() == id1) {
                person1 = person;
            }
            if (person.getId() == id2) {
                person2 = person;
            }
        }
        if (person1 == null || person2 == null) {
            throw new PersonIdNotFoundException();
        }
        return person1.getAge() - person2.getAge();
    }

    @Override
    public int compareName(int id1, int id2) throws PersonIdNotFoundException {
        Person person1 = null;
        Person person2 = null;
        for (Person person : people) {
            if (person.getId() == id1) {
                person1 = person;
            }
            if (person.getId() == id2) {
                person2 = person;
            }
        }
        if (person1 == null || person2 == null) {
            throw new PersonIdNotFoundException();
        }
        return person1.getName().compareTo(person2.getName());
    }

    @Override
    public int queryPeopleSum() {
        return people.size();
    }

    @Override
    public int queryNameRank(int id) throws PersonIdNotFoundException {
        Person person = null;
        for (Person pers : people) {
            if (pers.getId() == id) {
                person = pers;
            }
        }
        if (person == null) {
            throw new PersonIdNotFoundException();
        }
        int ret = 1;
        for (Person pers : people) {
            if (compareName(id, pers.getId()) > 0) {
                ret++;
            }
        }
        return ret;
    }

    @Override
    public boolean isCircle(int id1, int id2) throws PersonIdNotFoundException {
        Person person1 = null;
        Person person2 = null;
        for (Person person : people) {
            if (person.getId() == id1) {
                person1 = person;
            }
            if (person.getId() == id2) {
                person2 = person;
            }
        }
        if (person1 == null || person2 == null) {
            throw new PersonIdNotFoundException();
        }
        HashMap<Person, Boolean> visited = new HashMap<>();
        Queue<Person> queue = new LinkedList<>();
        queue.offer(person1);
        while (queue.peek() != null) {
            Person person = queue.poll();
            if (person.isLinked(person2)) {
                return true;
            }
            for (Person pers : ((MyPerson) person).getAcquaintance()) {
                if (!visited.containsKey(pers)) {
                    queue.offer(pers);
                    visited.put(pers, true);
                }
            }
        }
        return false;
    }

    /*private boolean isCircle(Person person1, Person person2) {
        if (!hm.containsKey(person1)) {
            hm.put(person1, true);
        } else {
            hm.replace(person1, true);
        }
        if (person1.isLinked(person2)) {
            hm.replace(person1, false);
            return true;
        }
        boolean ret = false;
        for (Person person : ((MyPerson) person1).getAcquaintance()) {
            if (!hm.containsKey(person) || !hm.get(person)) {
                ret = isCircle(person, person2);
                if (ret) {
                    break;
                }
            }
        }
        hm.replace(person1, false);
        return ret;
    }*/
}
