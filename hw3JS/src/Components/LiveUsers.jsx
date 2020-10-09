import React, { Component } from 'react';
import { Table } from 'reactstrap';

class LiveUsers extends Component {

    state = {
        visitors:[
            {
                ip: "localhost",
                first:"James",
                last: "Bond",
                user:"007"
            }
        ]
    }

    /* This will render teh table body.
    It does so by mapping the each visitor in the state array to a tabel row with each of it's properties.*/
    renderTableBody = () => {
        const {visitors} = this.state;
        return visitors.map((v, index) => {
            return (
                <tr>
                    <td>{index+ 1}</td>
                    <td>{v.ip}</td>
                    <td>{v.first}</td>
                    <td>{v.last}</td>
                    <td>{v.user}</td>
                </tr>
            )
        })
    }
    render () {
      return  (
          <React.Fragment>
            <h2>Live Users</h2>
            <Table>
            <thead>
            <tr>
                <th>#</th>
                <th>IP</th>
                <th>First Name</th>
                <th>Last Name</th>
                <th>Username</th>
            </tr>
            </thead>
            <tbody> {this.renderTableBody()} </tbody>
        </Table>
      </React.Fragment>
      )
    }
}


export default LiveUsers;