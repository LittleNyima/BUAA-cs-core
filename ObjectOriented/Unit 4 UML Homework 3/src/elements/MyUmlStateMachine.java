package elements;

import com.oocourse.uml3.interact.exceptions.user.StateDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.StateNotFoundException;
import com.oocourse.uml3.models.common.ElementType;
import com.oocourse.uml3.models.elements.UmlElement;

import java.util.ArrayList;

public class MyUmlStateMachine extends MyUmlElement {

    private final UmlElement umlStateMachine;
    private final ArrayList<MyUmlState> states;
    private int transitionCount;

    public MyUmlStateMachine(UmlElement umlStateMachine) {
        this.umlStateMachine = umlStateMachine;
        this.states = new ArrayList<>();
        this.transitionCount = 0;
    }

    @Override
    public int hashCode() {
        return umlStateMachine.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        } else if (o instanceof MyUmlStateMachine) {
            MyUmlStateMachine that = (MyUmlStateMachine) o;
            return this.umlStateMachine.equals(that.umlStateMachine);
        } else {
            return false;
        }
    }

    @Override
    public UmlElement getUmlElement() {
        return umlStateMachine;
    }

    @Override
    public ElementType getElementType() {
        return ElementType.UML_STATE_MACHINE;
    }

    @Override
    public String getId() {
        return umlStateMachine.getId();
    }

    @Override
    public String getName() {
        return umlStateMachine.getName();
    }

    @Override
    public String getParentId() {
        return umlStateMachine.getParentId();
    }

    public void addState(MyUmlState myUmlState) {
        this.states.add(myUmlState);
    }

    public void addTransition() {
        this.transitionCount++;
    }

    public int getStateCount() {
        return states.size();
    }

    public int getTransitionCount() {
        return transitionCount;
    }

    public int getSubsequentStateCount(String stateName)
            throws StateNotFoundException, StateDuplicatedException {
        MyUmlState target = null;
        for (MyUmlState state : states) {
            if (state.getName() != null && state.getName().equals(stateName)) {
                if (target == null) {
                    target = state;
                } else {
                    throw new StateDuplicatedException(this.umlStateMachine.getName(), stateName);
                }
            }
        }
        if (target == null) {
            throw new StateNotFoundException(this.umlStateMachine.getName(), stateName);
        }
        return target.getSubsequentStateCount();
    }
}
