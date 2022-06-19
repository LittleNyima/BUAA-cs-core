package elements;

import com.oocourse.uml3.interact.exceptions.user.LifelineDuplicatedException;
import com.oocourse.uml3.interact.exceptions.user.LifelineNotFoundException;
import com.oocourse.uml3.models.common.ElementType;
import com.oocourse.uml3.models.elements.UmlElement;

import java.util.ArrayList;

public class MyUmlInteraction extends MyUmlElement {

    private final UmlElement umlInteraction;
    private final ArrayList<MyUmlLifeline> participants;
    private int messageCount;

    public MyUmlInteraction(UmlElement umlInteraction) {
        this.umlInteraction = umlInteraction;
        this.participants = new ArrayList<>();
        this.messageCount = 0;
    }

    @Override
    public int hashCode() {
        return umlInteraction.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        } else if (o instanceof MyUmlInteraction) {
            MyUmlInteraction that = (MyUmlInteraction) o;
            return this.umlInteraction.equals(that.umlInteraction);
        } else {
            return false;
        }
    }

    @Override
    public UmlElement getUmlElement() {
        return umlInteraction;
    }

    @Override
    public ElementType getElementType() {
        return ElementType.UML_INTERACTION;
    }

    @Override
    public String getId() {
        return umlInteraction.getId();
    }

    @Override
    public String getName() {
        return umlInteraction.getName();
    }

    @Override
    public String getParentId() {
        return umlInteraction.getParentId();
    }

    public void addParticipant(MyUmlLifeline myUmlLifeline) {
        participants.add(myUmlLifeline);
    }

    public void addMessage() {
        messageCount++;
    }

    public int getParticipantCount() {
        return participants.size();
    }

    public int getMessageCount() {
        return messageCount;
    }

    public int getIncomingMessageCount(String lifelineName)
            throws LifelineNotFoundException, LifelineDuplicatedException {
        MyUmlLifeline target = null;
        for (MyUmlLifeline myUmlLifeline : participants) {
            if (myUmlLifeline.getName() != null && myUmlLifeline.getName().equals(lifelineName)) {
                if (target == null) {
                    target = myUmlLifeline;
                } else {
                    throw new LifelineDuplicatedException(
                            this.umlInteraction.getName(), lifelineName
                    );
                }
            }
        }
        if (target == null) {
            throw new LifelineNotFoundException(this.umlInteraction.getName(), lifelineName);
        }
        return target.getIncomingMessageCount();
    }
}
