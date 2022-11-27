#ifndef ORGANISM_H
#define ORGANISM_H

#include <tuple>
#include <optional>
#include <cassert>

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
requires std::equality_comparable<species_t>
class Organism {
public:
    constexpr Organism(const species_t& species, const uint64_t vitality)
        : species(species), vitality(vitality) {}

    constexpr const species_t &get_species() const {
        return species;
    }

    constexpr uint64_t get_vitality() const {
        return vitality;
    }

    constexpr bool is_dead() const {
        return vitality == 0;
    }

    static constexpr bool isCarnivore() {
        return can_eat_meat && !can_eat_plants;
    }

    static constexpr bool isOmnivore() {
        return can_eat_meat && can_eat_plants;
    }

    static constexpr bool isHerbivore() {
        return !can_eat_meat && can_eat_plants;
    }

    static constexpr bool isPlant() {
        return !can_eat_meat && !can_eat_plants;
    }

    static constexpr uint64_t fight(const uint64_t opponentsVitality) {
        return opponentsVitality / 2;
    }

    static constexpr uint64_t devour(const uint64_t opponentsVitality) {
        return opponentsVitality;
    }

    constexpr Organism<species_t, can_eat_meat, can_eat_plants>
    eat(const uint64_t preysVitality, uint64_t (*gain)(uint64_t)) const {
        Organism<species_t, can_eat_meat, can_eat_plants> predator =
            {Organism<species_t, can_eat_meat, can_eat_plants>(species, vitality + gain(preysVitality))};
        return predator;
    }

    constexpr Organism<species_t, can_eat_meat, can_eat_plants>
    die() const {
        Organism<species_t, can_eat_meat, can_eat_plants> prey =
            {Organism<species_t, can_eat_meat, can_eat_plants>(species, 0)};
        return prey;
    }
private:
    const species_t species;
    const uint64_t vitality;
};

template <typename species_t>
using Carnivore = Organism<species_t, true, false>;

template <typename species_t>
using Omnivore = Organism<species_t, true, true>;

template <typename species_t>
using Herbivore = Organism<species_t, false, true>;

template <typename species_t>
using Plant = Organism<species_t, false, false>;

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
                     Organism<species_t, sp2_eats_m, sp2_eats_p>,
                     std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {
    uint64_t (*fight)(uint64_t) = Organism<species_t, sp1_eats_m, sp1_eats_p>::fight;
    uint64_t (*devour)(uint64_t) = Organism<species_t, sp1_eats_m, sp1_eats_p>::devour;

    // If both are plants.
    static_assert(!(organism1.isPlant() && organism2.isPlant()));

    // If any is dead.
    if (organism1.is_dead() || organism2.is_dead()) {
        return std::make_tuple(Organism(organism1), Organism(organism2), std::nullopt);
    }

    // If species types are the same.
    if (organism1.get_species() == organism2.get_species()) {
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>> baby =
                {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(),
                                                             (organism1.get_vitality() +
                                                              organism2.get_vitality()) / 2)};
        return std::make_tuple(Organism(organism1), Organism(organism2), baby);
    }

    // If organisms can't eat each other.
    if ((organism1.isHerbivore() && organism2.isHerbivore()) ||
        (organism1.isCarnivore() && organism2.isPlant()) ||
        (organism1.isPlant() && organism2.isCarnivore())) {
        return std::make_tuple(Organism(organism1), Organism(organism2), std::nullopt);
    }

    // If both can eat each other.
    if ((organism1.isCarnivore() || organism1.isOmnivore()) && (organism2.isCarnivore() || organism2.isOmnivore())) {
        if (organism1.get_vitality() < organism2.get_vitality()) {
            return std::make_tuple(organism1.die(), organism2.eat(organism1.get_vitality(), fight), std::nullopt);
        } else if (organism1.get_vitality() > organism2.get_vitality()) {
            return std::make_tuple(organism1.eat(organism2.get_vitality(), fight), organism2.die(), std::nullopt);
        } else {
            return std::make_tuple(organism1.die(), organism2.die(), std::nullopt);
        }
    }

    // If Omnivore meets Plant or Herbivore meets Plant.
    if ((organism1.isPlant() || organism2.isPlant()) && !organism1.isCarnivore() && !organism2.isCarnivore()) {
        if (organism1.isPlant()) {
            return std::make_tuple(organism1.die(), organism2.eat(organism1.get_vitality(), devour), std::nullopt);
        } else {
            return std::make_tuple(organism1.eat(organism2.get_vitality(), devour), organism2.die(), std::nullopt);
        }
    }

    // If only one can eat another.
    if (organism1.isHerbivore()) {
        if (organism1.get_vitality() >= organism2.get_vitality()) {
            return std::make_tuple(Organism(organism1), Organism(organism2), std::nullopt);
        } else {
            return std::make_tuple(organism1.die(), organism2.eat(organism1.get_vitality(), fight), std::nullopt);
        }
    } else {
        if (organism2.get_vitality() >= organism1.get_vitality()) {
            return std::make_tuple(Organism(organism1), Organism(organism2), std::nullopt);
        } else {
            return std::make_tuple(organism1.eat(organism2.get_vitality(), fight), organism2.die(), std::nullopt);
        }
    }
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
operator+(const Organism<species_t, sp1_eats_m, sp1_eats_p>& organism1,
          const Organism<species_t, sp2_eats_m, sp2_eats_p>& organism2) {
    auto encounter_result = encounter(organism1, organism2);
    return get<0>(encounter_result);
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args ... args) {
    return (organism1 + ... + args);
}

#endif // ORGANISM_H
