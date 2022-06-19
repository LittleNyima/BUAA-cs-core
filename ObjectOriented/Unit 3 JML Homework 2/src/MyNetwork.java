import com.oocourse.spec2.exceptions.EqualPersonIdException;
import com.oocourse.spec2.exceptions.EqualRelationException;
import com.oocourse.spec2.exceptions.PersonIdNotFoundException;
import com.oocourse.spec2.exceptions.RelationNotFoundException;
import com.oocourse.spec2.exceptions.GroupIdNotFoundException;
import com.oocourse.spec2.exceptions.EqualGroupIdException;
import com.oocourse.spec2.main.Group;
import com.oocourse.spec2.main.Network;
import com.oocourse.spec2.main.Person;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Queue;

public class MyNetwork implements Network {

    private final HashMap<Integer, Person> people;
    private final HashMap<Integer, Group> groups;
    private final ArrayList<Person> peopleList;
    private final ArrayList<Group> groupList;

    public MyNetwork() {
        this.people = new HashMap<>();
        this.groups = new HashMap<>();
        this.peopleList = new ArrayList<>();
        this.groupList = new ArrayList<>();
    }

    @Override
    public boolean contains(int id) {
        return people.containsKey(id);
    }

    @Override
    public Person getPerson(int id) {
        return people.getOrDefault(id, null);
    }

    @Override
    public void addPerson(Person person) throws EqualPersonIdException {
        if (people.containsKey(person.getId())) {
            throw new EqualPersonIdException();
        } else {
            people.put(person.getId(), person);
            peopleList.add(person);
        }
    }

    @Override
    public void addRelation(int id1, int id2, int value) throws
            PersonIdNotFoundException, EqualRelationException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        if (id1 != id2 && people.get(id1).isLinked(people.get(id2))) {
            throw new EqualRelationException();
        }
        if (id1 == id2) {
            return;
        }
        ((MyPerson) people.get(id1)).addRelation(people.get(id2), value);
        ((MyPerson) people.get(id2)).addRelation(people.get(id1), value);
        for (Group group : groupList) {
            if (group.hasPerson(people.get(id1)) && group.hasPerson(people.get(id2))) {
                ((MyGroup) group).add2relation();
                ((MyGroup) group).add2value(value);
            }
        }
    }

    @Override
    public int queryValue(int id1, int id2) throws
            PersonIdNotFoundException, RelationNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        if (!people.get(id1).isLinked(people.get(id2))) {
            throw new RelationNotFoundException();
        }
        return people.get(id1).queryValue(people.get(id2));
    }

    @Override
    public BigInteger queryConflict(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id1).getCharacter().xor(people.get(id2).getCharacter());
    }

    @Override
    public int queryAcquaintanceSum(int id) throws PersonIdNotFoundException {
        if (!people.containsKey(id)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id).getAcquaintanceSum();
    }

    @Override
    public int compareAge(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id1).getAge() - people.get(id2).getAge();
    }

    @Override
    public int compareName(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id1).getName().compareTo(people.get(id2).getName());
    }

    @Override
    public int queryPeopleSum() {
        return people.size();
    }

    @Override
    public int queryNameRank(int id) throws PersonIdNotFoundException {
        if (!people.containsKey(id)) {
            throw new PersonIdNotFoundException();
        }
        int result = 1;
        for (Person person : peopleList) {
            if (compareName(id, person.getId()) > 0) {
                result++;
            }
        }
        return result;
    }

    @Override
    public boolean isCircle(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        HashMap<Person, Boolean> visited = new HashMap<>();
        Queue<Person> queue = new LinkedList<>();
        queue.offer(people.get(id1));
        while (queue.peek() != null) {
            Person person = queue.poll();
            if (person.isLinked(people.get(id2))) {
                return true;
            }
            for (MyRelation relation : ((MyPerson) person).getRelationList()) {
                if (!visited.containsKey(relation.getPerson())) {
                    queue.offer(relation.getPerson());
                    visited.put(relation.getPerson(), true);
                }
            }
        }
        return false;
    }

    @Override
    public void addGroup(Group group) throws EqualGroupIdException {
        if (groups.containsKey(group.getId())) {
            throw new EqualGroupIdException();
        }
        groups.put(group.getId(), group);
        groupList.add(group);
    }

    @Override
    public Group getGroup(int id) {
        return groups.get(id);
    }

    @Override
    public void addtoGroup(int id1, int id2) throws GroupIdNotFoundException,
            PersonIdNotFoundException, EqualPersonIdException {
        if (!groups.containsKey(id2)) {
            throw new GroupIdNotFoundException();
        }
        if (!people.containsKey(id1)) {
            throw new PersonIdNotFoundException();
        }
        if (groups.get(id2).hasPerson(people.get(id1))) {
            throw new EqualPersonIdException();
        }
        if (((MyGroup) groups.get(id2)).size() < 1111) {
            groups.get(id2).addPerson(people.get(id1));
        }
    }

    @Override
    public int queryGroupSum() {
        return groups.size();
    }

    @Override
    public int queryGroupPeopleSum(int id) throws GroupIdNotFoundException {
        if (!groups.containsKey(id)) {
            throw new GroupIdNotFoundException();
        }
        return ((MyGroup) groups.get(id)).size();
    }

    @Override
    public int queryGroupRelationSum(int id) throws GroupIdNotFoundException {
        if (!groups.containsKey(id)) {
            throw new GroupIdNotFoundException();
        }
        return groups.get(id).getRelationSum();
    }

    @Override
    public int queryGroupValueSum(int id) throws GroupIdNotFoundException {
        if (!groups.containsKey(id)) {
            throw new GroupIdNotFoundException();
        }
        return groups.get(id).getValueSum();
    }

    @Override
    public BigInteger queryGroupConflictSum(int id) throws GroupIdNotFoundException {
        if (!groups.containsKey(id)) {
            throw new GroupIdNotFoundException();
        }
        return groups.get(id).getConflictSum();
    }

    @Override
    public int queryGroupAgeMean(int id) throws GroupIdNotFoundException {
        if (!groups.containsKey(id)) {
            throw new GroupIdNotFoundException();
        }
        return groups.get(id).getAgeMean();
    }

    @Override
    public int queryGroupAgeVar(int id) throws GroupIdNotFoundException {
        if (!groups.containsKey(id)) {
            throw new GroupIdNotFoundException();
        }
        return groups.get(id).getAgeVar();
    }
}
