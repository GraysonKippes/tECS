# tECS

tECS is a tiny ECS written in C.

Current version: 0.1.

## Usage

Begin by defining a component bitmask type with COMPONENT_MASK_TYPE. The number of bits in this type limits how many component-types can be accessed with a signature. By default, COMPONENT_MASK_TYPE is defined as uint8_t, meaning that eight component-types can be accessed.

Next, include component_registry.h and use the register_component_type macro to register component_types.

Next, include archetype.h and create the archetypes needed for the program, using the component masks.

Next, include entity_manager.h and call init_entity_manager to initialize the manager, then create entities with the archetypes as needed.

Make sure to free any archetypes and entities when no longer needed.

## Design

### Entity

Entities consist of opaque handles, a numeric type called `entity_t`. By default `entity_t` is `unsigned int`, but this can be changed by defining `ENTITY_TYPE` with the new type.

Entities are provided and returned with the `entity_manager` module. The entity manager keeps tracks of which entities are available, and which are currently borrowed by the user. The maximum number of entities available can be defined by `MAX_NUM_ENTITIES`; by default, this is set to 256.

### Component

A component is an arbitrary, user-defined data type that is registered with the ECS. An entity can have any or all component-types registered with it.

### Signature

The signature of an entity is a bitmask indicating which component-types that entity has. Because of the positional nature of this, component-types have to be sequentially registered.

For example, if vector3D_t position is component type 0, double weight is component type 1, and unsigned int age is component type 2, then the bitmask 0b011 describes an entity with position and weight, but no age. Likewise, the bitmask 0b101 describes an entity with position and age, but no weight.

### Archetype

An archetype is a table of the components which belong to all entities of the same signature. The table consists of one or more columns, which are arrays of components of the same type.

### Record

A record contains a pointer to the archetype in which an entity's components are stored, as well as the row at which the components are stored. Row refers to the index across all columns in the archetype at which each of the entity's components may be found.
