package elements;

import com.oocourse.uml1.models.common.ElementType;
import com.oocourse.uml1.models.elements.UmlElement;

public class MyUmlOthers extends MyUmlElement {

    private final UmlElement umlElement;

    public MyUmlOthers(UmlElement umlElement) {
        this.umlElement = umlElement;
    }

    @Override
    public int hashCode() {
        return umlElement.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        } else if (o instanceof MyUmlOthers) {
            MyUmlOthers that = (MyUmlOthers) o;
            return this.umlElement.equals(that.umlElement);
        } else {
            return false;
        }
    }

    @Override
    public UmlElement getUmlElement() {
        return umlElement;
    }

    @Override
    public ElementType getElementType() {
        return umlElement.getElementType();
    }

    @Override
    public String getId() {
        return umlElement.getId();
    }

    @Override
    public String getName() {
        return umlElement.getName();
    }

    @Override
    public String getParentId() {
        return umlElement.getParentId();
    }
}
