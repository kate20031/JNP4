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

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
constexpr bool isCarnivore(Organism<species_t, can_eat_meat, can_eat_plants>&) {
    return can_eat_meat && !can_eat_plants;
}

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
constexpr bool isOmnivore(Organism<species_t, can_eat_meat, can_eat_plants>&) {
    return can_eat_meat && can_eat_plants;
}

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
constexpr bool isHerbivore(Organism<species_t, can_eat_meat, can_eat_plants>&) {
    return !can_eat_meat && can_eat_plants;
}

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
constexpr bool isPlant(Organism<species_t, can_eat_meat, can_eat_plants>&) {
    return !can_eat_meat && !can_eat_plants;
}

constexpr uint64_t fight(uint64_t a, uint64_t b) {
    return a + b / 2;
}

constexpr uint64_t profit(uint64_t a, uint64_t b) {
    return a + b;
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::pair<Organism<species_t, sp1_eats_m, sp1_eats_p>,
                    Organism<species_t, sp2_eats_m, sp2_eats_p>>
eat(Organism<species_t, sp1_eats_m, sp1_eats_p> predator,
    Organism<species_t, sp2_eats_m, sp2_eats_p> prey,
    uint64_t (*gain)(uint64_t, uint64_t)) {
    Organism<species_t, sp1_eats_m, sp1_eats_p> fedPredator =
        {Organism<species_t, sp1_eats_m, sp1_eats_p>(predator.get_species(),
                                                     gain(predator.get_vitality(),
                                                          prey.get_vitality()))};
    Organism<species_t, sp2_eats_m, sp2_eats_p> deadPrey =
        {Organism<species_t, sp2_eats_m, sp2_eats_p>(prey.get_species(), 0)};
    return std::make_pair(fedPredator, deadPrey);
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
                     Organism<species_t, sp2_eats_m, sp2_eats_p>,
                     std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {

    // If both are plants.
    static_assert(!(isPlant(organism1) && isPlant(organism2)));

    // If any is dead.
    if (organism1.is_dead() || organism2.is_dead()) {
        return std::make_tuple(Organism(organism1), Organism(organism2), std::nullopt);
    }

    // If species types are the same then it leads to mating.
    if (organism1.get_species() == organism2.get_species()) {
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>> baby =
                {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(),
                                                             (organism1.get_vitality() +
                                                              organism2.get_vitality()) / 2)};
        return std::make_tuple(Organism(organism1), Organism(organism2), baby);
    }

    // If organisms can't eat each other then there are no results of meeting.
    if ((isHerbivore(organism1) && isHerbivore(organism2)) ||
        (isCarnivore(organism1) && isPlant(organism2)) ||
        (isPlant(organism1) && isCarnivore(organism2))) {
        return std::make_tuple(Organism(organism1), Organism(organism2), std::nullopt);
    }

    // If both of organisms are animals.
    if (!isPlant(organism1) && !isPlant(organism2)) {
        if (organism1.get_vitality() < organism2.get_vitality()) {
            auto modifiedOrganisms = eat(organism2, organism1, fight);
            return std::make_tuple(modifiedOrganisms.second, modifiedOrganisms.first, std::nullopt);
        } else if (organism1.get_vitality() > organism2.get_vitality()) {
            auto modifiedOrganisms = eat(organism1, organism2, fight);
            return std::make_tuple(modifiedOrganisms.first, modifiedOrganisms.second, std::nullopt);
        } else {
            Organism<species_t, sp1_eats_m, sp1_eats_p> modifiedOrganism1 =
                    {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(), 0)};
            Organism<species_t, sp2_eats_m, sp2_eats_p> modifiedOrganism2 =
                    {Organism<species_t, sp2_eats_m, sp2_eats_p>(organism2.get_species(), 0)};
            return std::make_tuple(modifiedOrganism1, modifiedOrganism2, std::nullopt);
        }
    }

    // If Omnivore meets Plant or Herbivore meets Plant.
    if ((isPlant(organism1) || isPlant(organism2)) && (!isCarnivore(organism1) && !isCarnivore(organism2))) {
        if (isPlant(organism1)) {
            auto modifiedOrganisms = eat(organism2, organism1, profit);
            return std::make_tuple(modifiedOrganisms.second, modifiedOrganisms.first, std::nullopt);
        } else {
            auto modifiedOrganisms = eat(organism1, organism2, profit);
            return std::make_tuple(modifiedOrganisms.first, modifiedOrganisms.second, std::nullopt);
        }
    }

    // If only one of organisms can eat another.
    if (isHerbivore(organism1)) {
        if (organism1.get_vitality() >= organism2.get_vitality()) {
            return std::make_tuple(Organism(organism1), Organism(organism2), std::nullopt);
        } else {
            auto modifiedOrganisms = eat(organism2, organism1, fight);
            return std::make_tuple(modifiedOrganisms.second, modifiedOrganisms.first, std::nullopt);
        }
    } else {
        if (organism2.get_vitality() >= organism1.get_vitality()) {
            return std::make_tuple(Organism(organism1), Organism(organism2), std::nullopt);
        } else {
            auto modifiedOrganisms = eat(organism1, organism2, fight);
            return std::make_tuple(modifiedOrganisms.first, modifiedOrganisms.second, std::nullopt);
        }
    }
}

/*
template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args ... args) {
}
*/

#endif // ORGANISM_H
