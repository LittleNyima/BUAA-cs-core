package elements;

import com.oocourse.uml1.models.common.ElementType;
import com.oocourse.uml1.models.elements.UmlElement;
import interfaces.Generalizable;

import java.util.ArrayList;

public class MyUmlInterface extends MyUmlElement implements Generalizable {

    private final UmlElement umlInterface;
    private final ArrayList<MyUmlInterface> parents;

    public MyUmlInterface(UmlElement umlInterface) {
        this.umlInterface = umlInterface;
        this.parents = new ArrayList<>();
    }

    @Override
    public int hashCode() {
        return umlInterface.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (o == this) {
            return true;
        } else if (o instanceof MyUmlInterface) {
            MyUmlInterface that = (MyUmlInterface) o;
            return this.umlInterface.equals(that.umlInterface);
        } else {
            return false;
        }
    }

    @Override
    public UmlElement getUmlElement() {
        return this.umlInterface;
    }

    @Override
    public ElementType getElementType() {
        return ElementType.UML_INTERFACE;
    }

    @Override
    public String getId() {
        return umlInterface.getId();
    }

    @Override
    public String getName() {
        return umlInterface.getName();
    }

    @Override
    public String getParentId() {
        return umlInterface.getParentId();
    }

    @Override
    public void addParent(Generalizable parent) {
        this.parents.add((MyUmlInterface) parent);
    }

    public ArrayList<MyUmlInterface> getParents() {
        return this.parents;
    }
}
