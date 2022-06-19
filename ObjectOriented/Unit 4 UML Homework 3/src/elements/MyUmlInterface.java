package elements;

import com.oocourse.uml3.models.common.ElementType;
import com.oocourse.uml3.models.common.Visibility;
import com.oocourse.uml3.models.elements.UmlAttribute;
import com.oocourse.uml3.models.elements.UmlElement;
import interfaces.Generalizable;

import java.util.ArrayList;

public class MyUmlInterface extends MyUmlClassOrInterface implements Generalizable {

    private final UmlElement umlInterface;
    private final ArrayList<UmlAttribute> attributes;
    private final ArrayList<MyUmlOperation> operations;
    private final ArrayList<MyUmlInterface> parents;

    public MyUmlInterface(UmlElement umlInterface) {
        this.umlInterface = umlInterface;
        this.attributes = new ArrayList<>();
        this.operations = new ArrayList<>();
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

    public void addOperation(MyUmlOperation operation) {
        this.operations.add(operation);
    }

    public void addAttribute(UmlAttribute attribute) {
        this.attributes.add(attribute);
    }

    public ArrayList<MyUmlInterface> getParents() {
        return this.parents;
    }

    public boolean allAttributeIsPublic() {
        for (UmlAttribute attribute : attributes) {
            if (attribute.getVisibility() != Visibility.PUBLIC) {
                return false;
            }
        }
        return true;
    }

    @Override
    public ArrayList<MyUmlClassOrInterface> getParentsClassOrInterface() {
        return new ArrayList<>(parents);
    }
}
