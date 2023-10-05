# tECS

tECS is a tiny ECS written in C.

Current version: `0.2`

## Usage

Begin by including `tECS/tecs.h`. Then register your component types with `register_component_type` and create your archetypes with `create_archetype`.

After that, call `init_entity_manager` to initialize the entity manager; now you can create entities with `create_entity`.

Once you are done with an entity, you can free it with `free_entity`.

Make sure to free any archetypes you create with `free_archetype`.

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
