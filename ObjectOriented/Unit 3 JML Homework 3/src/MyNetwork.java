import com.oocourse.spec3.exceptions.EqualGroupIdException;
import com.oocourse.spec3.exceptions.EqualPersonIdException;
import com.oocourse.spec3.exceptions.EqualRelationException;
import com.oocourse.spec3.exceptions.GroupIdNotFoundException;
import com.oocourse.spec3.exceptions.PersonIdNotFoundException;
import com.oocourse.spec3.exceptions.RelationNotFoundException;
import com.oocourse.spec3.main.Group;
import com.oocourse.spec3.main.Network;
import com.oocourse.spec3.main.Person;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.PriorityQueue;
import java.util.Queue;

public class MyNetwork implements Network {

    private final HashMap<Integer, MyBalance> people;
    private final HashMap<Integer, Group> groups;
    private final ArrayList<Person> peopleList;
    private final ArrayList<Group> groupList;
    private final UnionFindSet relations;

    public MyNetwork() {
        this.people = new HashMap<>();
        this.groups = new HashMap<>();
        this.peopleList = new ArrayList<>();
        this.groupList = new ArrayList<>();
        this.relations = new UnionFindSet();
    }

    @Override
    public boolean contains(int id) {
        return people.containsKey(id);
    }

    @Override
    public Person getPerson(int id) {
        if (!people.containsKey(id)) {
            return null;
        } else {
            return people.get(id).getPerson();
        }
    }

    @Override
    public void addPerson(Person person) throws EqualPersonIdException {
        if (people.containsKey(person.getId())) {
            throw new EqualPersonIdException();
        } else {
            people.put(person.getId(), new MyBalance(person, 0));
            peopleList.add(person);
            relations.addElement(person.getId());
        }
    }

    @Override
    public void addRelation(int id1, int id2, int value) throws
            PersonIdNotFoundException, EqualRelationException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        if (id1 != id2 && people.get(id1).getPerson().isLinked(people.get(id2).getPerson())) {
            throw new EqualRelationException();
        }
        if (id1 == id2) {
            return;
        }
        ((MyPerson) people.get(id1).getPerson()).addRelation(people.get(id2).getPerson(), value);
        ((MyPerson) people.get(id2).getPerson()).addRelation(people.get(id1).getPerson(), value);
        relations.addLink(id1, id2);
        for (Group group : groupList) {
            if (group.hasPerson(people.get(id1).getPerson()) &&
                    group.hasPerson(people.get(id2).getPerson())) {
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
        if (!people.get(id1).getPerson().isLinked(people.get(id2).getPerson())) {
            throw new RelationNotFoundException();
        }
        return people.get(id1).getPerson().queryValue(people.get(id2).getPerson());
    }

    @Override
    public BigInteger queryConflict(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id1).getPerson()
                .getCharacter().xor(people.get(id2).getPerson().getCharacter());
    }

    @Override
    public int queryAcquaintanceSum(int id) throws PersonIdNotFoundException {
        if (!people.containsKey(id)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id).getPerson().getAcquaintanceSum();
    }

    @Override
    public int compareAge(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id1).getPerson().getAge() - people.get(id2).getPerson().getAge();
    }

    @Override
    public int compareName(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id1).getPerson()
                .getName().compareTo(people.get(id2).getPerson().getName());
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
        return relations.isLinked(id1, id2);
        /*HashMap<Person, Boolean> visited = new HashMap<>();
        Queue<Person> queue = new LinkedList<>();
        queue.offer(people.get(id1).getPerson());
        while (queue.peek() != null) {
            Person person = queue.poll();
            if (person.isLinked(people.get(id2).getPerson())) {
                return true;
            }
            for (MyRelation relation : ((MyPerson) person).getRelationList()) {
                if (!visited.containsKey(relation.getPerson())) {
                    queue.offer(relation.getPerson());
                    visited.put(relation.getPerson(), true);
                }
            }
        }
        return false;*/
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
        if (groups.get(id2).hasPerson(people.get(id1).getPerson())) {
            throw new EqualPersonIdException();
        }
        if (((MyGroup) groups.get(id2)).size() < 1111) {
            groups.get(id2).addPerson(people.get(id1).getPerson());
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

    @Override
    public int queryAgeSum(int l, int r) {
        int ret = 0;
        for (Person person : peopleList) {
            if (person.getAge() >= l && person.getAge() <= r) {
                ret++;
            }
        }
        return ret;
    }

    @Override
    public void delFromGroup(int id1, int id2)
            throws GroupIdNotFoundException, PersonIdNotFoundException, EqualPersonIdException {
        if (!groups.containsKey(id2)) {
            throw new GroupIdNotFoundException();
        }
        if (!people.containsKey(id1)) {
            throw new PersonIdNotFoundException();
        }
        if (!groups.get(id2).hasPerson(people.get(id1).getPerson())) {
            throw new EqualPersonIdException();
        }
        groups.get(id2).delPerson(people.get(id1).getPerson());
    }

    @Override
    public int queryMinPath(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        } else if (id1 == id2) {
            return 0;
        } else if (!isCircle(id1, id2)) {
            return -1;
        }
        HashMap<Person, Boolean> settled = new HashMap<>();
        HashMap<Person, Integer> distance = new HashMap<>();
        for (Person person : peopleList) {
            distance.put(person, Integer.MAX_VALUE);
        }
        Queue<MyRelation> queue = new PriorityQueue<>();
        queue.offer(new MyRelation(people.get(id1).getPerson(), 0));
        distance.replace(people.get(id1).getPerson(), 0);
        while (!queue.isEmpty()) {
            if (settled.containsKey(people.get(id2).getPerson())) {
                break;
            }
            MyRelation cur = queue.poll();
            if (cur.getValue() < distance.get(cur.getPerson()) ||
                    settled.containsKey(cur.getPerson())) {
                continue;
            }
            settled.put(cur.getPerson(), true);
            ArrayList<MyRelation> acquaintance = ((MyPerson) cur.getPerson()).getRelationList();
            for (MyRelation myRelation : acquaintance) {
                Person person = myRelation.getPerson();
                int value = myRelation.getValue();
                int newVal = distance.get(cur.getPerson()) + value;
                if (!settled.containsKey(person) &&
                        distance.get(person) > newVal) {
                    distance.replace(person, newVal);
                    queue.offer(new MyRelation(person, distance.get(person)));
                }
            }
        }
        return distance.get(people.get(id2).getPerson());
    }

    @Override
    public boolean queryStrongLinked(int id1, int id2) throws PersonIdNotFoundException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        } else if (id1 == id2) {
            return true;
        } else if (!isCircle(id1, id2)) {
            return false;
        } else if (people.get(id1).getPerson().isLinked(people.get(id2).getPerson())) {
            return queryLongPath(id1, id2);
        } else {
            for (Person person : peopleList) {
                if (person.getId() != id1 && person.getId() != id2 &&
                        isCircle(person.getId(), id1) && isCircle(person.getId(), id2)) {
                    if (!queryCircleWithEscape(id1, id2, person.getId())) {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    private boolean queryLongPath(int id1, int id2) {
        HashMap<Person, Boolean> visited = new HashMap<>();
        Queue<Person> queue = new LinkedList<>();
        queue.offer(people.get(id1).getPerson());
        visited.put(people.get(id1).getPerson(), true);
        while (queue.peek() != null) {
            Person person = queue.poll();
            if (person.getId() == id1) {
                for (MyRelation relation : ((MyPerson) person).getRelationList()) {
                    if (!visited.containsKey(relation.getPerson()) &&
                            relation.getPerson().getId() != id2) {
                        queue.offer(relation.getPerson());
                        visited.put(relation.getPerson(), true);
                    }
                }
            } else {
                if (person.isLinked(people.get(id2).getPerson())) {
                    return true;
                }
                for (MyRelation relation : ((MyPerson) person).getRelationList()) {
                    if (!visited.containsKey(relation.getPerson())) {
                        queue.offer(relation.getPerson());
                        visited.put(relation.getPerson(), true);
                    }
                }
            }
        }
        return false;
    }

    private boolean queryCircleWithEscape(int id1, int id2, int escape) {
        HashMap<Person, Boolean> visited = new HashMap<>();
        Queue<Person> queue = new LinkedList<>();
        queue.offer(people.get(id1).getPerson());
        visited.put(people.get(id1).getPerson(), true);
        visited.put(people.get(escape).getPerson(), true);
        while (queue.peek() != null) {
            Person person = queue.poll();
            if (person.isLinked(people.get(id2).getPerson())) {
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
    public int queryBlockSum() {
        return relations.blockSum();
    }

    @Override
    public void borrowFrom(int id1, int id2, int value)
            throws PersonIdNotFoundException, EqualPersonIdException {
        if (!people.containsKey(id1) || !people.containsKey(id2)) {
            throw new PersonIdNotFoundException();
        }
        if (id1 == id2) {
            throw new EqualPersonIdException();
        }
        people.get(id1).withdraw(value);
        people.get(id2).saveMoney(value);
    }

    @Override
    public int queryMoney(int id) throws PersonIdNotFoundException {
        if (!people.containsKey(id)) {
            throw new PersonIdNotFoundException();
        }
        return people.get(id).getMoney();
    }
}
