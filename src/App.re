type repository = {
  name: string,
  description: string,
  href: string,
};

type state = {
  input: string,
  loading: bool,
  results: list(repository),
};

type action =
  | UpdateInput(string)
  | UpdateResults(list(repository))
  | Search;

let component = ReasonReact.reducerComponent("App");

module Api = {
  open Json.Decode;
  module Promise = Js.Promise;

  let decodeResults =
    field(
      "items",
      list(
        optional(json =>
          {
            name: field("name", string, json),
            description: field("description", string, json),
            href: field("html_url", string, json),
          }
        ),
      ),
    );

  let getResults = query =>
    /*
     * This is similar to `open Json.Decode`, it allows the Promise functions
     * to be available within the parentheses
     */
    Promise.(
      Fetch.fetch("https://api.github.com/search/repositories?q=" ++ query)
      |> Promise.then_(Fetch.Response.json)
      |> Promise.then_(json => decodeResults(json) |> resolve)
      |> Promise.then_(results =>
           results
           |> List.filter(optionalItem =>
                switch (optionalItem) {
                | Some(_) => true
                | None => false
                }
              )
           |> List.map(item =>
                switch (item) {
                | Some(item) => item
                }
              )
           |> resolve
         )
    );
};

module Styles = {
  open Css;

  let list = style([
    display(`flex),
    flexWrap(`wrap),
  ])
}

let make = _children => {
  ...component,
  initialState: () => {input: "", loading: false, results: []},
  reducer: (action, state) =>
    switch (action) {
    | UpdateInput(input) => ReasonReact.Update({...state, input})
    | UpdateResults(results) =>
      ReasonReact.Update({...state, loading: false, results})
    | Search =>
      ReasonReact.UpdateWithSideEffects(
        {...state, loading: true},
        (
          self => {
            let value = self.state.input;
            let _promise =
              Api.getResults(value)
              |> Js.Promise.then_(results => {
                   self.send(UpdateResults(results));
                   Js.Promise.resolve();
                 });
            ();
          }
        ),
      )
    },
  render: self =>
    <div>
      <form
        onSubmit={
          ev => {
            ReactEvent.Form.preventDefault(ev);
            self.send(Search);
          }
        }>
        <label htmlFor="search"> {ReasonReact.string("Search")} </label>
        <input
          id="search"
          name="search"
          value={self.state.input}
          onChange={
            ev => {
              let value = ReactEvent.Form.target(ev)##value;
              self.send(UpdateInput(value));
            }
          }
        />
        <button type_="submit">
          {ReasonReact.string("Submit Search")}
        </button>
      </form>
      <div>
        {
          self.state.loading ?
            ReasonReact.string("Loading...") :
            self.state.results
            |> Array.of_list
            |> Array.map(({name, href, description}) =>
                 <Card key={name} name href description />
               )
            |> ReasonReact.array
        }
      </div>
    </div>,
};