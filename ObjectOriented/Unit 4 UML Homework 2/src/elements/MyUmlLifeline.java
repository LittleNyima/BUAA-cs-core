package elements;

import com.oocourse.uml2.models.common.ElementType;
import com.oocourse.uml2.models.elements.UmlElement;

public class MyUmlLifeline extends MyUmlElement {

    private final UmlElement umlLifeline;
    private int incomingCount;

    public MyUmlLifeline(UmlElement umlLifeline) {
        this.umlLifeline = umlLifeline;
        this.incomingCount = 0;
    }

    @Override
    public int hashCode() {
        return umlLifeline.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        } else if (o instanceof MyUmlLifeline) {
            MyUmlLifeline that = (MyUmlLifeline) o;
            return this.umlLifeline.equals(that.umlLifeline);
        } else {
            return false;
        }
    }

    @Override
    public UmlElement getUmlElement() {
        return umlLifeline;
    }

    @Override
    public ElementType getElementType() {
        return ElementType.UML_LIFELINE;
    }

    @Override
    public String getId() {
        return umlLifeline.getId();
    }

    @Override
    public String getName() {
        return umlLifeline.getName();
    }

    @Override
    public String getParentId() {
        return umlLifeline.getParentId();
    }

    public void addIncomingMessage() {
        incomingCount++;
    }

    public int getIncomingMessageCount() {
        return incomingCount;
    }
}
