#ifndef ORGANISM_H
#define ORGANISM_H

#include <tuple>
#include <optional>
#include <cassert>

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
class Organism {
public:
    constexpr Organism(species_t const &species, uint64_t vitality)
            : species(species), vitality(vitality) {
        static_assert(std::equality_comparable<species_t>);
    }

    constexpr const species_t &get_species() const {
        return species;
    }

    constexpr uint64_t get_vitality() const {
        return vitality;
    }

    constexpr bool is_dead() const {
        return vitality == 0;
    }
private:
    const species_t species;
    uint64_t vitality;
};

template <typename species_t>
using Carnivore = Organism<species_t, true, false>;

template <typename species_t>
using Omnivore = Organism<species_t, true, true>;

template <typename species_t>
using Herbivore = Organism<species_t, false, true>;

template <typename species_t>
using Plant = Organism<species_t, false, false>;

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p>
constexpr bool isPlant(Organism<species_t, sp1_eats_m, sp1_eats_p> organism) {
    return (!sp1_eats_m && !sp1_eats_p);
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p>
constexpr bool isCarnivore(Organism<species_t, sp1_eats_m, sp1_eats_p> organism) {
    return (sp1_eats_m && !sp1_eats_p);
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p>
constexpr bool isHerbivore(Organism<species_t, sp1_eats_m, sp1_eats_p> organism) {
    return (!sp1_eats_m && sp1_eats_p);
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p>
constexpr bool isOmnivore(Organism<species_t, sp1_eats_m, sp1_eats_p> organism) {
    return (sp1_eats_m && sp1_eats_p);
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
        Organism<species_t, sp2_eats_m, sp2_eats_p>,
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
eatAnimals(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2, bool firstEats) {
    if (firstEats) {
        Organism<species_t, sp1_eats_m, sp1_eats_p> modifiedOrganism1 =
                {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(),
                             (organism1.get_vitality() + organism2.get_vitality() / 2))};
        Organism<species_t, sp2_eats_m, sp2_eats_p> modifiedOrganism2 =
                {Organism<species_t, sp2_eats_m, sp2_eats_p>(organism2.get_species(), 0)};
        return std::make_tuple(modifiedOrganism1, modifiedOrganism2, std::nullopt);
    }
    else {
        Organism<species_t, sp1_eats_m, sp1_eats_p> modifiedOrganism1 =
                {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(), 0)};
        Organism<species_t, sp2_eats_m, sp2_eats_p> modifiedOrganism2 =
                {Organism<species_t, sp2_eats_m, sp2_eats_p>(organism2.get_species(),
                         (organism2.get_vitality() + organism1.get_vitality() / 2))};
        return std::make_tuple(modifiedOrganism1, modifiedOrganism2, std::nullopt);
    }
}


template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
        Organism<species_t, sp2_eats_m, sp2_eats_p>,
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {

//     Check if species type of organisms are the same.
//    static_assert(sp1_eats_m == sp2_eats_m && sp1_eats_p == sp2_eats_p);

    // Check if both organisms aren't plants.
    static_assert(sp1_eats_m || sp1_eats_p || sp2_eats_m || sp2_eats_p);

    // Check if both of organisms are alive.
    if (organism1.is_dead() || organism2.is_dead()) {
        return std::make_tuple(Organism(organism1),
                               Organism(organism2),
                               std::nullopt);
    }

    // If species type of organisms are the same, it leads to mating.
    if (organism1.get_species() == organism2.get_species()) {
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>> child =
                {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(),
                                                             (organism1.get_vitality() + organism2.get_vitality()) / 2)};
        return std::make_tuple(Organism(organism1),
                               Organism(organism2),
                               child);
    }

    // if organisms can't eat each other, there are no results of meeting.
    if (isPlant(organism1) || isPlant(organism2) || isHerbivore(organism1) || isHerbivore(organism2)) {
        return std::make_tuple(Organism(organism1),
                               Organism(organism2),
                               std::nullopt);
    }

    // If both of organisms are animals.
    if (!(isPlant(organism1) && isPlant(organism2))) {
        bool firstEats;
        if (organism1.get_vitality() < organism2.get_vitality()) {
            firstEats = false;

            return eatAnimals(organism1, organism2, firstEats);

        } else if (organism1.get_vitality() > organism2.get_vitality()) {
            firstEats = true;

            return eatAnimals(organism1, organism2, firstEats);
        } else {
            Organism<species_t, sp1_eats_m, sp1_eats_p> modifiedOrganism1 =
                    {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(), 0)};
            Organism<species_t, sp2_eats_m, sp2_eats_p> modifiedOrganism2 =
                    {Organism<species_t, sp2_eats_m, sp2_eats_p>(organism2.get_species(), 0)};

            return std::make_tuple(modifiedOrganism1, modifiedOrganism2, std::nullopt);
        }
    }

    // If Omnivore meets Plant or Herbivore meets Plant.
    if (isPlant(organism1) && isOmnivore(organism2) || isPlant(organism2) && isOmnivore(organism1)
        || isPlant(organism1) && isHerbivore(organism2) || isPlant(organism2) && isHerbivore(organism1)) {

        if (isPlant(organism1)) {
            Organism<species_t, sp1_eats_m, sp1_eats_p> modifiedOrganism1 =
                    {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(), 0)};
            Organism<species_t, sp2_eats_m, sp2_eats_p> modifiedOrganism2 =
                        {Organism<species_t, sp2_eats_m, sp2_eats_p>(organism2.get_species(),
                        (organism2.get_vitality() + organism1.get_vitality()))};

            return std::make_tuple(modifiedOrganism1, modifiedOrganism2, std::nullopt);

        } else {
            Organism<species_t, sp1_eats_m, sp1_eats_p>
                    modifiedOrganism1 = {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(),
                                         (organism1.get_vitality() + organism2.get_vitality()))};
            Organism<species_t, sp2_eats_m, sp2_eats_p> modifiedOrganism2 =
                    {Organism<species_t, sp2_eats_m, sp2_eats_p>(organism2.get_species(), 0)};

            return std::make_tuple(modifiedOrganism1, modifiedOrganism2, std::nullopt);
        }
    }

    // If only one of organisms can eat another.
    if ((isHerbivore(organism1) && (isOmnivore(organism2) || isCarnivore(organism2)))
        || (isHerbivore(organism2) && (isOmnivore(organism1) || isCarnivore(organism1)))) {

        if (isHerbivore(organism1)) {
            if (organism1.get_vitality() >= organism2.get_vitality()) {
                return std::make_tuple(Organism(organism1),
                                       Organism(organism2), std::nullopt);
            } else {
                return eatAnimals(organism1, organism2, false);
            }
        } else if (isHerbivore(organism2)) {
            if (organism2.get_vitality() >= organism1.get_vitality()) {
                return std::make_tuple(Organism(organism1),
                                       Organism(organism2), std::nullopt);
            } else {
                return eatAnimals(organism1, organism2, true);
            }
        }
    }

    return std::make_tuple(Organism(organism1),
                           Organism(organism2),
                           std::nullopt);
}

/*
template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args ... args) {
}
*/

#endif // ORGANISM_H