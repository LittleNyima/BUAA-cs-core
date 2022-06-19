package elements;

import com.oocourse.uml3.models.common.ElementType;
import com.oocourse.uml3.models.elements.UmlElement;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Queue;

public class MyUmlState extends MyUmlElement {

    private final UmlElement umlState;
    private final ArrayList<MyUmlState> subsequences;

    public MyUmlState(UmlElement umlState) {
        this.umlState = umlState;
        this.subsequences = new ArrayList<>();
    }

    @Override
    public int hashCode() {
        return umlState.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        } else if (o instanceof MyUmlState) {
            MyUmlState that = (MyUmlState) o;
            return this.umlState.equals(that.umlState);
        } else {
            return false;
        }
    }

    @Override
    public UmlElement getUmlElement() {
        return this.umlState;
    }

    @Override
    public ElementType getElementType() {
        return umlState.getElementType();
    }

    @Override
    public String getId() {
        return umlState.getId();
    }

    @Override
    public String getName() {
        return umlState.getName();
    }

    @Override
    public String getParentId() {
        return umlState.getParentId();
    }

    public void addSubsequence(MyUmlState myUmlState) {
        subsequences.add(myUmlState);
    }

    public int getSubsequentStateCount() {
        HashSet<MyUmlState> flag = new HashSet<>();
        Queue<MyUmlState> queue = new LinkedList<>();
        queue.offer(this);
        while (!queue.isEmpty()) {
            MyUmlState state = queue.poll();
            for (MyUmlState subsequence : state.subsequences) {
                if (!flag.contains(subsequence)) {
                    flag.add(subsequence);
                    queue.offer(subsequence);
                }
            }
        }
        return flag.size();
    }

    public int getDirectSubsequentStateCount() {
        return subsequences.size();
    }
}
