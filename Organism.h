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

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
                     Organism<species_t, sp2_eats_m, sp2_eats_p>,
                     std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {
    // 1.
    static_assert(sp1_eats_m == sp2_eats_m && sp1_eats_p == sp2_eats_p);

    // 2.
    static_assert(sp1_eats_m || sp1_eats_p || sp2_eats_m || sp2_eats_p);

    // 3.
    if (organism1.is_dead() || organism2.is_dead()) {
        return std::make_tuple(Organism(organism1),
                               Organism(organism2),
                               std::nullopt);
    }

    // 4.
    if (organism1.get_species() == organism2.get_species()) {
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>> child =
        {Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(),
        (organism1.get_vitality() + organism2.get_vitality()) / 2)};
        return std::make_tuple(Organism(organism1),
                               Organism(organism2),
                               child);
    }

    // 5.


    // 6.


    // 7.


    // 8.



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
